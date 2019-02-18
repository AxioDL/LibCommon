#ifndef NCONVEXHULL_H
#define NCONVEXHULL_H

#include "CPlane.h"
#include "CVector3f.h"

/** Utilities for creating a convex hull from a point cloud */
namespace NConvexHull
{

/** Generate vertices for a convex hull */
bool BuildConvexHullVertices( const std::vector<CVector3f>& kInPoints,
                              std::vector<CVector3f>& OutVertices );

/** Generate face planes for a convex hull */
bool BuildConvexHullPlanes( const std::vector<CVector3f>& kInPoints,
                            std::vector<CPlane>& OutPlanes );

/** Generate mesh data for a convex hull */
bool BuildConvexHullMesh( const std::vector<CVector3f>& kInPoints,
                          std::vector<CVector3f>& OutVertices,
                          std::vector<uint16>& OutTriangleIndices );

}

#endif // NCONVEXHULL_H
