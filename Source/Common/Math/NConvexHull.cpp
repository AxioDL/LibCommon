#include "NConvexHull.h"
#include "MathUtil.h"
#include "Common/CScopedTimer.h"
#include "Common/Macros.h"
#include "Common/NBasics.h"

#include <cfloat>
#include <vector>

namespace NConvexHull
{

/** Quickhull implementation for building convex hulls */
class CQuickhullImpl
{
    struct SVertex;
    struct SHalfEdge;
    struct SFace;

    /** A convex hull vertex */
    struct SVertex
    {
        // The vertex position
        CVector3f Position;
        // Conflict face for this vertex (a hull face that is visible from this vertex)
        SFace* pConflictFace;
        // Squared distance from conflict face
        float ConflictDistance;
    };

    /** A convex hull half-edge */
    struct SHalfEdge
    {
        // The first vertex in the edge
        uint Tail;
        // Circularly linked list of edges in the face
        SHalfEdge* pPrev;
        SHalfEdge* pNext;
        // The mirror of this edge on the opposing face
        SHalfEdge* pTwin;
        // The face this edge belongs to
        SFace* pFace;
    };

    /** A convex hull face */
    struct SFace
    {
        // Linked list of faces in the hull
        SFace* pPrev;
        SFace* pNext;
        // Circular linked list of edges in the face
        SHalfEdge* pFirstEdge;
        // Face plane
        CPlane Plane;
        // "Visited" flag for horizon check
        bool Visited;
    };

    /** Input vertex data. This data is generated at the start of the algorithm and not modified.
     *  As such, this array represents the input data, and may contain vertices not used in the output hull. */
    std::vector<SVertex> mVertices;

    /** Face linked list tail */
    SFace* mpFaceTail;

    /** The number of faces in the hull */
    uint mNumFaces;
    
    /** Threshold for distance comparisons (epsilon) */
    float mEpsilon;

    /** True if the algorithm completed successfully */
    bool mSuccess;

    /** Create and initialize a new empty face with no edges */
    SFace* CreateFace()
    {
        SFace* pFace = new SFace;
        pFace->pPrev = mpFaceTail;
        pFace->pNext = nullptr;
        pFace->pFirstEdge = nullptr;
        pFace->Visited = false;

        if (mpFaceTail)
            mpFaceTail->pNext = pFace;

        mpFaceTail = pFace;
        mNumFaces++;
        return pFace;
    }

    /** Delete a face from the hull. This function can leave dangling pointers. */
    void DeleteFace(SFace* pFace)
    {
        if (pFace->pPrev)
            pFace->pPrev->pNext = pFace->pNext;

        if (pFace->pNext)
            pFace->pNext->pPrev = pFace->pPrev;

        if (pFace == mpFaceTail)
            mpFaceTail = pFace->pPrev;

        SHalfEdge* pEdge = pFace->pFirstEdge;
        do
        {
            SHalfEdge* pNext = pEdge->pNext;
            delete pEdge;
            pEdge = pNext;
        }
        while (pEdge != pFace->pFirstEdge);

        delete pFace;
        mNumFaces--;
    }
    
    /** Creates and initializes a half edge. */
    SHalfEdge* CreateHalfEdge(SFace* pFace, uint TailVertexIdx, SHalfEdge* pPrev, SHalfEdge* pNext, SHalfEdge* pTwin)
    {
        SHalfEdge* pEdge = new SHalfEdge;
        pEdge->Tail = TailVertexIdx;
        pEdge->pTwin = pTwin;
        pEdge->pPrev = pPrev;
        pEdge->pNext = pNext;
        pEdge->pFace = pFace;
        
        if (pPrev)
            pPrev->pNext = pEdge;
        if (pNext)
            pNext->pPrev = pEdge;
        if (pTwin)
            pTwin->pTwin = pEdge;
        if (pFace && !pFace->pFirstEdge)
            pFace->pFirstEdge = pEdge;
        
        return pEdge;
    }
    
    /** Calculate and set the plane that a face lies on */
    void CalcFacePlane(SFace* pFace)
    {
        ASSERT( pFace->pFirstEdge != nullptr );
        ASSERT( pFace->pFirstEdge->pNext != nullptr );
        ASSERT( pFace->pFirstEdge->pNext->pNext != nullptr );
        
        uint Idx0 = pFace->pFirstEdge->Tail;
        uint Idx1 = pFace->pFirstEdge->pNext->Tail;
        uint Idx2 = pFace->pFirstEdge->pNext->pNext->Tail;
        const CVector3f& V0 = mVertices[Idx0].Position;
        const CVector3f& V1 = mVertices[Idx1].Position;
        const CVector3f& V2 = mVertices[Idx2].Position;
        const CVector3f V0to1 = (V1 - V0);
        const CVector3f V1to2 = (V2 - V1);
        const CVector3f Norm = V0to1.Cross(V1to2).Normalized();
        
        pFace->Plane = CPlane(Norm, V0);
    }
    
    /** Calculate the centroid of a face */
    CVector3f CalcFaceCentroid(SFace* pFace)
    {
        CVector3f Out;
        uint NumVertices = 0;

        SHalfEdge* pEdge = pFace->pFirstEdge;
        do
        {
            Out += mVertices[pEdge->Tail].Position;
            NumVertices++;
            pEdge = pEdge->pNext;
        }
        while (pEdge != pFace->pFirstEdge);

        return Out;
    }

    /** Initialize face conflict data on all vertices. */
    void FillFaceConflicts()
    {
        for (uint i=0; i<mVertices.size(); i++)
        {
            SVertex& V = mVertices[i];
            V.pConflictFace = nullptr;
            V.ConflictDistance = FLT_MAX;

            for (SFace* pFace = mpFaceTail; pFace; pFace = pFace->pPrev)
            {
                float Dist = pFace->Plane.DistanceFromPoint(V.Position);

                if (Dist < -mEpsilon && Math::Abs(Dist) < V.ConflictDistance)
                {
                    V.ConflictDistance = Math::Abs(Dist);
                    V.pConflictFace = pFace;
                }
            }
        }
    }

    /** Generate the initial tetrahedron for the hull */
    bool InitHull(const std::vector<CVector3f>& kInPoints)
    {
        // We need at least 4 input vertices to create a hull. If this is not the case, fail out.
        if (kInPoints.size() < 4)
        {
            return false;
        }

        // Generate vertices
        mVertices.resize(kInPoints.size());

        for (uint i=0; i<kInPoints.size(); i++)
        {
            mVertices[i].Position = kInPoints[i];
            mVertices[i].pConflictFace = nullptr;
            mVertices[i].ConflictDistance = 0.f;
        }

        // Identify the most extremal vertices along each cardinal axis.
        // These will be the first vertices to be added to the hull.
        const CVector3f& V0 = mVertices[0].Position;
        int MinXIdx = 0, MinYIdx = 0, MinZIdx = 0, MaxXIdx = 0, MaxYIdx = 0, MaxZIdx = 0;
        float MinX = V0.X, MinY = V0.Y, MinZ = V0.Z, MaxX = V0.X, MaxY = V0.Y, MaxZ = V0.Z;

        for (int i=1; i<kInPoints.size(); i++)
        {
            const CVector3f& V = kInPoints[i];
            if (V.X < MinX)
            {
                MinX = V.X;
                MinXIdx = i;
            }
            if (V.X > MaxX)
            {
                MaxX = V.X;
                MaxXIdx = i;
            }
            if (V.Y < MinY)
            {
                MinY = V.Y;
                MinYIdx = i;
            }
            if (V.Y > MaxY)
            {
                MaxY = V.Y;
                MaxYIdx = i;
            }
            if (V.Z < MinZ)
            {
                MinZ = V.Z;
                MinZIdx = i;
            }
            if (V.Z > MaxZ)
            {
                MaxZ = V.Z;
                MaxZIdx = i;
            }
        }

        // Now we have the the most extremal points along each axis - determine which are furthest apart
        int Point0, Point1;
        float XRange = MaxX - MinX;
        float YRange = MaxY - MinY;
        float ZRange = MaxZ - MinZ;

        if (XRange > YRange && XRange > ZRange)
        {
            Point0 = MinXIdx;
            Point1 = MaxXIdx;
        }
        else if (YRange > XRange && YRange > ZRange)
        {
            Point0 = MinYIdx;
            Point1 = MaxYIdx;
        }
        else
        {
            Point0 = MinZIdx;
            Point1 = MaxZIdx;
        }

        // Also, calculate our epsilon threshold we will use for various distance comparisons
        float AbsX = Math::Max( Math::Abs(MinX), Math::Abs(MaxX) );
        float AbsY = Math::Max( Math::Abs(MinY), Math::Abs(MaxY) );
        float AbsZ = Math::Max( Math::Abs(MinZ), Math::Abs(MaxZ) );
        mEpsilon = (AbsX + AbsY + AbsZ) * FLT_EPSILON * 2.f;

        // We now have a line segment defined by Point0 and Point1.
        // Determine which remaining vertex is furthest from this line.
        const CVector3f& kP0 = kInPoints[Point0];
        const CVector3f& kP1 = kInPoints[Point1];
        const CVector3f& kLineNormal = (kP1 - kP0).Normalized();
        float MaxSqDist = 0.f;
        int MaxIdx = -1;

        for (int i=0; i<kInPoints.size(); i++)
        {
            if (i != Point0 && i != Point1)
            {
                const CVector3f& V = kInPoints[i];
                float SqBase = (V - kP0).Dot(kLineNormal);
                float SqHypotenuse = kP0.SquaredDistance(V);
                float SqDist = SqHypotenuse - SqBase;

                if (SqDist > MaxSqDist)
                {
                    MaxSqDist = SqDist;
                    MaxIdx = i;
                }
            }
        }

        int Point2 = MaxIdx;
        const CVector3f& kP2 = kInPoints[Point2];

        // Now find the point which is furthest from this plane to form a tetrahedron
        // This is the initial hull from which we can expand and add more vertices
        const CVector3f& kPlaneNormal = (kP1-kP0).Cross(kP2-kP1).Normalized();
        const CPlane kBasePlane(kPlaneNormal, kP0);
        float MaxDist = 0.f;
        MaxIdx = -1;

        for (int i=0; i<kInPoints.size(); i++)
        {
            if (i != Point0 && i != Point1 && i != Point2)
            {
                const CVector3f& V = kInPoints[i];
                float Dist = Math::Abs( kBasePlane.DistanceFromPoint(V) );

                if (Dist > MaxDist)
                {
                    MaxDist = Dist;
                    MaxIdx = i;
                }
            }
        }

        // This is an edge case that can occur if all input points are planar
        if (MaxIdx == -1 || MaxDist <= mEpsilon)
        {
            return false;
        }

        int Point3 = MaxIdx;
        const CVector3f& kP3 = kInPoints[Point3];

        // Check which side of the initial triangle point 3 is on to determine if we need to flip winding
        if (kBasePlane.DistanceFromPoint(kP3) < 0.f)
        {
            int Tmp = Point2;
            Point2 = Point0;
            Point0 = Tmp;
        }
        
        // Now we can construct the first faces of the hull.
        // Base: 0,1,2
        SFace* pBase = CreateFace();
        SHalfEdge* pEdge01 = CreateHalfEdge(pBase, Point0, 0, 0, 0);
        SHalfEdge* pEdge12 = CreateHalfEdge(pBase, Point1, pEdge01, 0, 0);
        SHalfEdge* pEdge20 = CreateHalfEdge(pBase, Point2, pEdge12, pEdge01, 0);
        CalcFacePlane(pBase);
        // Side0: 0,3,1
        SFace* pSide0 = CreateFace();
        SHalfEdge* pEdge03 = CreateHalfEdge(pSide0, Point0, 0, 0, 0);
        SHalfEdge* pEdge31 = CreateHalfEdge(pSide0, Point3, pEdge03, 0, 0);
        SHalfEdge* pEdge10 = CreateHalfEdge(pSide0, Point1, pEdge31, pEdge03, pEdge01);
        CalcFacePlane(pSide0);
        // Side1: 3,2,1
        SFace* pSide1 = CreateFace();
        SHalfEdge* pEdge32 = CreateHalfEdge(pSide1, Point3, 0, 0, 0);
        SHalfEdge* pEdge21 = CreateHalfEdge(pSide1, Point2, pEdge32, 0, pEdge12);
        SHalfEdge* pEdge13 = CreateHalfEdge(pSide1, Point1, pEdge21, pEdge32, pEdge31);
        CalcFacePlane(pSide1);
        // Side2: 0,2,3
        SFace* pSide2 = CreateFace();
        SHalfEdge* pEdge02 = CreateHalfEdge(pSide2, Point0, 0, 0, pEdge20);
        SHalfEdge* pEdge23 = CreateHalfEdge(pSide2, Point2, pEdge02, 0, pEdge32);
        SHalfEdge* pEdge30 = CreateHalfEdge(pSide2, Point3, pEdge23, pEdge02, pEdge03);
        CalcFacePlane(pSide2);

        // Calculate initial face conflicts, then clear conflict data for vertices in the initial hull
        FillFaceConflicts();
        mVertices[Point0].pConflictFace = nullptr;
        mVertices[Point1].pConflictFace = nullptr;
        mVertices[Point2].pConflictFace = nullptr;
        mVertices[Point3].pConflictFace = nullptr;

        return true;
    }

    /** Look for the next vertex to add to the hull. Returns false when hull is complete. */
    bool ResolveNextConflict()
    {
        // Look for the conflict vertex with the largest distance from the hull.
        // This is the next vertex that will be added to the hull.
        uint VertexIndex = 0xFFFFFFFF;
        float MaxDist = 0.f;

        for (uint i=0; i<mVertices.size(); i++)
        {
            if (mVertices[i].pConflictFace != nullptr &&
                mVertices[i].ConflictDistance > MaxDist)
            {
                VertexIndex = i;
                MaxDist = mVertices[i].ConflictDistance;
            }
        }

        // No vertex found? All conflicts are resolved
        if (VertexIndex == 0xFFFFFFFF)
            return false;

        AddToHull(VertexIndex);
        return true;
    }

    /** Recursive function for building a list of horizon edges for a given vertex */
    void RecursiveBuildHorizon(SVertex* pVertex, SHalfEdge* pFirstEdge, std::vector<SFace*>& OutVisibleFaces, std::vector<SHalfEdge*>& OutHorizon)
    {
        // Ensure we don't visit the same face multiple times
        SFace* pFace = pFirstEdge->pFace;

        if (pFace->Visited)
            return;

        pFace->Visited = true;
        OutVisibleFaces.push_back(pFace);

        // Depth-first iteration of all edges
        SHalfEdge* pEdge = pFirstEdge;

        do
        {
            // Check if the face on the other side of the edge is visible.
            if (pEdge->pTwin->pFace->Plane.DistanceFromPoint(pVertex->Position) < -mEpsilon)
            {
                RecursiveBuildHorizon(pVertex, pEdge->pTwin, OutVisibleFaces, OutHorizon);
            }
            else
            {
                OutHorizon.push_back(pEdge);
            }
            pEdge = pEdge->pNext;
        }
        while (pEdge != pFirstEdge);
    }

    /** Adds a new vertex to the convex hull */
    void AddToHull(uint VertexIndex)
    {
        SVertex& Vertex = mVertices[VertexIndex];

        std::vector<SHalfEdge*> Horizon;
        std::vector<SFace*> VisibleFaces;
        RecursiveBuildHorizon(&Vertex, Vertex.pConflictFace->pFirstEdge, VisibleFaces, Horizon);

        // With a list of edges and faces, we can now expand the hull to include the new vertex.
        // Start by creating a new face linking each horizon edge to the new vertex.
        std::vector<SFace*> NewFaces(Horizon.size());
        SHalfEdge* pFirstNewSide = nullptr;
        SHalfEdge* pLastNewSide = nullptr;

        for (uint EdgeIdx = 0; EdgeIdx < Horizon.size(); EdgeIdx++)
        {
            SHalfEdge* pEdge = Horizon[EdgeIdx];
            uint V0 = pEdge->Tail;
            uint V1 = pEdge->pTwin->Tail;
            uint V2 = VertexIndex;

            if (V0 == V1 || V1 == V2 || V2 == V0)
            {
                errorf("Quickhull: Degenerate face being constructed: %d/%d/%d", V0, V1, V2);
            }

            // Disassociate the edge's twin from the edge, since it is being deleted soon
            SHalfEdge* pOldTwin = pEdge->pTwin;
            pEdge->pTwin = nullptr;
            pOldTwin->pTwin = nullptr;

            // Create the new face
            SFace* pFace = CreateFace();
            SHalfEdge* pEdge01 = CreateHalfEdge(pFace, V0, 0, 0, pOldTwin);
            SHalfEdge* pEdge12 = CreateHalfEdge(pFace, V1, pEdge01, 0, 0);
            SHalfEdge* pEdge20 = CreateHalfEdge(pFace, V2, pEdge12, pEdge01, pLastNewSide);
            CalcFacePlane(pFace);
            NewFaces[EdgeIdx] = pFace;

            if (EdgeIdx == 0)
            {
                pFirstNewSide = pEdge20;
            }
            pLastNewSide = pEdge12;
        }

        pFirstNewSide->pTwin = pLastNewSide;
        pLastNewSide->pTwin = pFirstNewSide;

        // Clear conflict data from the vertex, since it is now part of the hull.
        Vertex.pConflictFace = nullptr;
        Vertex.ConflictDistance = FLT_MAX;

        // Assign old conflicts to one of the new faces, then delete old faces/edges
        for (uint FaceIdx = 0; FaceIdx < VisibleFaces.size(); FaceIdx++)
        {
            SFace* pOldFace = VisibleFaces[FaceIdx];

            for (uint VertexIdx = 0; VertexIdx < mVertices.size(); VertexIdx++)
            {
                if (mVertices[VertexIdx].pConflictFace  == pOldFace)
                {
                    // Found a conflict. Remap to one of the new faces.
                    SVertex& V = mVertices[VertexIdx];
                    V.pConflictFace = nullptr;
                    V.ConflictDistance = FLT_MAX;

                    for (uint NewFaceIdx = 0; NewFaceIdx < NewFaces.size(); NewFaceIdx++)
                    {
                        float Dist = NewFaces[NewFaceIdx]->Plane.DistanceFromPoint(V.Position);

                        if (Dist < -mEpsilon && Math::Abs(Dist) < V.ConflictDistance)
                        {
                            V.ConflictDistance = Math::Abs(Dist);
                            V.pConflictFace = NewFaces[NewFaceIdx];
                        }
                    }
                }
            }

            DeleteFace(pOldFace);
        }
    }

public:
    /** Quickhull entry point; instantiating the class will run the algorithm */
    CQuickhullImpl(const std::vector<CVector3f>& kInPoints)
        : mEpsilon(FLT_EPSILON)
        , mpFaceTail(nullptr)
        , mNumFaces(0)
        , mSuccess(false)
    {
        SCOPED_TIMER(Quickhull);

        if (InitHull(kInPoints))
        {
            while (ResolveNextConflict()) {}
            mSuccess = true;
        }
    }

    ~CQuickhullImpl()
    {
        while (mpFaceTail)
        {
            DeleteFace(mpFaceTail);
        }
    }

    /** Returns whether the algorithm completed successfully */
    inline bool CompletedSuccessfully() const
    {
        return mSuccess;
    }

    /** Retrieve output data from the algorithm */
    void GetHullVertices(std::vector<CVector3f>& OutVertices) const
    {
        // Note that our internal vertex array matches the input vertices, not the output vertices.
        // Iterate all faces to get a list of used vertices.
        OutVertices.reserve(mVertices.size());
        SFace* pFace = mpFaceTail;

        while (pFace)
        {
            SHalfEdge* pEdge = pFace->pFirstEdge;

            do
            {
                NBasics::VectorAddUnique(OutVertices, mVertices[pEdge->Tail].Position);
                pEdge = pEdge->pNext;
            }
            while (pEdge != pFace->pFirstEdge);
        }
    }

    void GetHullPlanes(std::vector<CPlane>& OutPlanes) const
    {
        // This is a slight hack due to the fact that there is currently no face merging
        OutPlanes.reserve(mNumFaces);
        SFace* pFace = mpFaceTail;

        while (pFace)
        {
            NBasics::VectorAddUnique(OutPlanes, pFace->Plane);
            pFace = pFace->pPrev;
        }

        OutPlanes.shrink_to_fit();
    }

    void GetHullTriangles(std::vector<CVector3f>& OutVertices,
                      std::vector<uint16>& OutTriangleIndices) const
    {
        // Note that our internal vertex array matches the input vertices, not the output vertices.
        // Iterate all faces to get a list of used vertices and generate triangle data.
        // This function assumes that faces may have more than 3 edges.
        OutVertices.reserve(mVertices.size());
        OutTriangleIndices.reserve(mNumFaces * 3);
        SFace* pFace = mpFaceTail;

        while (pFace)
        {
            SHalfEdge* pEdge = pFace->pFirstEdge;
            uint16 FirstVert = 0xFFFF;
            uint16 LastVert = 0xFFFF;

            do
            {
                const SVertex& Vertex = mVertices[pEdge->Tail];
                uint16 VertexIndex = (uint16) NBasics::VectorAddUnique(OutVertices, Vertex.Position);

                if (FirstVert == 0xFFFF)
                {
                    FirstVert = VertexIndex;
                }
                else if (LastVert != 0xFFFF)
                {
                    OutTriangleIndices.push_back(FirstVert);
                    OutTriangleIndices.push_back(LastVert);
                    OutTriangleIndices.push_back(VertexIndex);
                }

                LastVert = VertexIndex;
                pEdge = pEdge->pNext;
            }
            while (pEdge != pFace->pFirstEdge);

            pFace = pFace->pPrev;
        }

        OutVertices.shrink_to_fit();
        OutTriangleIndices.shrink_to_fit();
    }
};

/** Generate vertices for a convex hull */
bool BuildConvexHullVertices( const std::vector<CVector3f>& kInPoints,
                              std::vector<CVector3f>& OutVertices )
{
    CQuickhullImpl Quickhull(kInPoints);

    if (Quickhull.CompletedSuccessfully())
    {
        Quickhull.GetHullVertices(OutVertices);
        return true;
    }
    else
        return false;
}

/** Generate face planes for a convex hull */
bool BuildConvexHullPlanes( const std::vector<CVector3f>& kInPoints,
                            std::vector<CPlane>& OutPlanes )
{
    CQuickhullImpl Quickhull(kInPoints);

    if (Quickhull.CompletedSuccessfully())
    {
        Quickhull.GetHullPlanes(OutPlanes);
        return true;
    }
    else
        return false;
}

/** Generate mesh data for a convex hull */
bool BuildConvexHullMesh( const std::vector<CVector3f>& kInPoints,
                          std::vector<CVector3f>& OutVertices,
                          std::vector<uint16>& OutTriangleIndices )
{
    CQuickhullImpl Quickhull(kInPoints);

    if (Quickhull.CompletedSuccessfully())
    {
        Quickhull.GetHullTriangles(OutVertices, OutTriangleIndices);
        return true;
    }
    else
        return false;
}

}
