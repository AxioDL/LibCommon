#include "CFrustumPlanes.h"
#include "MathUtil.h"
#include "Common/BasicTypes.h"

CFrustumPlanes::CFrustumPlanes()
{
}

const CPlane& CFrustumPlanes::GetPlane(EFrustumSide Side) const
{
    return mPlanes[Side];
}

void CFrustumPlanes::SetPlanes(const CVector3f& rkPosition, const CVector3f& rkDirection, float FieldOfView, float AspectRatio, float Near, float Far)
{
    // Calculate up/right vectors
    CVector3f Right = rkDirection.Cross(CVector3f::skUp).Normalized();
    CVector3f Up = Right.Cross(rkDirection).Normalized();

    // Calculate dimensions of near plane
    float NearHeight = 2 * tanf(Math::DegreesToRadians(FieldOfView) / 2.f) * Near;
    float NearWidth = NearHeight * AspectRatio;

    // Define the planes
    CVector3f NearCenter = rkPosition + (rkDirection * Near);
    mPlanes[kNearPlane].Redefine(rkDirection, NearCenter);

    CVector3f FarCenter = rkPosition + (rkDirection * Far);
    mPlanes[kFarPlane].Redefine(-rkDirection, FarCenter);

    CVector3f MidRight = NearCenter + (Right * (NearWidth / 2.f));
    CVector3f RightNormal = Up.Cross((MidRight - rkPosition).Normalized());
    mPlanes[kRightPlane].Redefine(RightNormal, rkPosition);

    CVector3f MidLeft = NearCenter - (Right * (NearWidth / 2.f));
    CVector3f LeftNormal = (MidLeft - rkPosition).Normalized().Cross(Up);
    mPlanes[kLeftPlane].Redefine(LeftNormal, rkPosition);

    CVector3f MidTop = NearCenter + (Up * (NearHeight / 2.f));
    CVector3f TopNormal = (MidTop - rkPosition).Normalized().Cross(Right);
    mPlanes[kTopPlane].Redefine(TopNormal, rkPosition);

    CVector3f MidBottom = NearCenter - (Up * (NearHeight / 2.f));
    CVector3f BottomNormal = Right.Cross((MidBottom - rkPosition).Normalized());
    mPlanes[kBottomPlane].Redefine(BottomNormal, rkPosition);
}

bool CFrustumPlanes::PointInFrustum(const CVector3f& rkPoint) const
{
    for (uint32 iPlane = 0; iPlane < 6; iPlane++)
    {
        const CPlane& rkPlane = mPlanes[iPlane];

        if (rkPlane.Normal().Dot(rkPoint) + rkPlane.Dist() < 0.f)
            return false;
    }

    return true;
}

bool CFrustumPlanes::BoxInFrustum(const CAABox& rkBox) const
{
    CVector3f Min = rkBox.Min();
    CVector3f Max = rkBox.Max();

    CVector3f Points[8];
    Points[0] = Min;
    Points[1] = Max;
    Points[2] = CVector3f(Min.X, Min.Y, Max.Z);
    Points[3] = CVector3f(Min.X, Max.Y, Min.Z);
    Points[4] = CVector3f(Min.X, Max.Y, Max.Z);
    Points[5] = CVector3f(Max.X, Min.Y, Max.Z);
    Points[6] = CVector3f(Max.X, Max.Y, Min.Z);
    Points[7] = CVector3f(Max.X, Min.Y, Min.Z);

    for (int PlaneIdx = 0; PlaneIdx < 6; PlaneIdx++)
    {
        const CPlane& rkPlane = mPlanes[PlaneIdx];
        int NumPoints = 0;

        for (int PointIdx = 0; PointIdx < 8; PointIdx++)
        {
            if (rkPlane.Normal().Dot(Points[PointIdx]) + rkPlane.Dist() < 0.f)
                NumPoints++;
            else
                break;
        }

        if (NumPoints == 8) return false;
    }
    return true;
}
