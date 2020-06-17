#include "CFrustumPlanes.h"
#include "MathUtil.h"
#include "Common/BasicTypes.h"
#include <algorithm>

static constexpr size_t SideToIndex(CFrustumPlanes::ESide side)
{
    return static_cast<size_t>(side);
}

CFrustumPlanes::CFrustumPlanes() = default;

const CPlane& CFrustumPlanes::GetPlane(ESide Side) const
{
    return mPlanes[SideToIndex(Side)];
}

void CFrustumPlanes::SetPlanes(const CVector3f& rkPosition, const CVector3f& rkDirection, float FieldOfView, float AspectRatio, float Near, float Far)
{
    // Calculate up/right vectors
    const CVector3f Right = rkDirection.Cross(CVector3f::Up()).Normalized();
    const CVector3f Up = Right.Cross(rkDirection).Normalized();

    // Calculate dimensions of near plane
    const float NearHeight = 2 * tanf(Math::DegreesToRadians(FieldOfView) / 2.f) * Near;
    const float NearWidth = NearHeight * AspectRatio;

    // Define the planes
    const CVector3f NearCenter = rkPosition + (rkDirection * Near);
    mPlanes[SideToIndex(ESide::NearPlane)].Redefine(rkDirection, NearCenter);

    const CVector3f FarCenter = rkPosition + (rkDirection * Far);
    mPlanes[SideToIndex(ESide::FarPlane)].Redefine(-rkDirection, FarCenter);

    const CVector3f MidRight = NearCenter + (Right * (NearWidth / 2.f));
    const CVector3f RightNormal = Up.Cross((MidRight - rkPosition).Normalized());
    mPlanes[SideToIndex(ESide::RightPlane)].Redefine(RightNormal, rkPosition);

    const CVector3f MidLeft = NearCenter - (Right * (NearWidth / 2.f));
    const CVector3f LeftNormal = (MidLeft - rkPosition).Normalized().Cross(Up);
    mPlanes[SideToIndex(ESide::LeftPlane)].Redefine(LeftNormal, rkPosition);

    const CVector3f MidTop = NearCenter + (Up * (NearHeight / 2.f));
    const CVector3f TopNormal = (MidTop - rkPosition).Normalized().Cross(Right);
    mPlanes[SideToIndex(ESide::TopPlane)].Redefine(TopNormal, rkPosition);

    const CVector3f MidBottom = NearCenter - (Up * (NearHeight / 2.f));
    const CVector3f BottomNormal = Right.Cross((MidBottom - rkPosition).Normalized());
    mPlanes[SideToIndex(ESide::BottomPlane)].Redefine(BottomNormal, rkPosition);
}

bool CFrustumPlanes::PointInFrustum(const CVector3f& point) const
{
    return std::all_of(mPlanes.cbegin(), mPlanes.cend(), [&point](const auto& entry) {
        return entry.Normal().Dot(point) + entry.Dist() >= 0.f;
    });
}

bool CFrustumPlanes::BoxInFrustum(const CAABox& rkBox) const
{
    const CVector3f Min = rkBox.Min();
    const CVector3f Max = rkBox.Max();

    const std::array Points{
        Min,
        Max,
        CVector3f(Min.X, Min.Y, Max.Z),
        CVector3f(Min.X, Max.Y, Min.Z),
        CVector3f(Min.X, Max.Y, Max.Z),
        CVector3f(Max.X, Min.Y, Max.Z),
        CVector3f(Max.X, Max.Y, Min.Z),
        CVector3f(Max.X, Min.Y, Min.Z),
    };

    for (const auto& plane : mPlanes)
    {
        int NumPoints = 0;

        for (const auto& point : Points)
        {
            if (plane.Normal().Dot(point) + plane.Dist() < 0.f)
                NumPoints++;
            else
                break;
        }

        if (NumPoints == 8)
            return false;
    }
    return true;
}
