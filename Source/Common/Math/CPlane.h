#ifndef CPLANE_H
#define CPLANE_H

#include "CVector3f.h"

class CPlane
{
    CVector3f mNormal{CVector3f::Up()};
    float mDist = 0.0f;

public:
    constexpr CPlane() = default;

    constexpr CPlane(const CVector3f& normal, float dist)
        : mNormal(normal)
        , mDist(dist)
    {}

    constexpr CPlane(const CVector3f& normal, const CVector3f& origin)
    {
        Redefine(normal, origin);
    }

    constexpr void Redefine(const CVector3f& normal, const CVector3f& origin)
    {
        mNormal = normal;
        mDist = -normal.Dot(origin);
    }

    [[nodiscard]] constexpr float DistanceFromPoint(const CVector3f& point) const
    {
        return point.Dot(mNormal) + mDist;
    }

    [[nodiscard]] constexpr bool operator==(const CPlane& other) const
    {
        return mNormal == other.mNormal && mDist == other.mDist;
    }

    [[nodiscard]] constexpr bool operator!=(const CPlane& other) const
    {
        return !operator==(other);
    }

    [[nodiscard]] constexpr CVector3f Normal() const  { return mNormal; }
    [[nodiscard]] constexpr float Dist() const        { return mDist; }
    constexpr void SetNormal(const CVector3f& normal) { mNormal = normal; }
    constexpr void SetDist(float dist)                { mDist = dist; }
};

#endif // CPLANE_H
