#ifndef CRAY_H
#define CRAY_H

#include "CTransform4f.h"
#include "CVector3f.h"

class CRay
{
    CVector3f mOrigin;
    CVector3f mDirection;

public:
    constexpr CRay() = default;

    constexpr CRay(const CVector3f& origin, const CVector3f& direction)
        : mOrigin(origin), mDirection(direction) {}

    [[nodiscard]] constexpr const CVector3f& Origin() const    { return mOrigin; }
    [[nodiscard]] constexpr const CVector3f& Direction() const { return mDirection; }
    constexpr void SetOrigin(const CVector3f& origin)          { mOrigin = origin; }
    constexpr void SetDirection(const CVector3f& direction)    { mDirection = direction; }

    [[nodiscard]] CRay Transformed(const CTransform4f& matrix) const
    {
        CRay Out;
        Out.mOrigin = matrix * mOrigin;

        CVector3f Point = matrix * (mOrigin + mDirection);
        Out.mDirection = (Point - Out.mOrigin).Normalized();

        return Out;
    }

    constexpr CVector3f PointOnRay(float distance) const { return mOrigin + (mDirection * distance); }
};

#endif // CRAY_H
