#ifndef AXIO_CFRUSTUMPLANES_H
#define AXIO_CFRUSTUMPLANES_H

#include "CPlane.h"
#include <array>

class CAABox;
class CVector3f;

class CFrustumPlanes
{
private:
    std::array<CPlane, 6> mPlanes;

public:
    enum class ESide
    {
        NearPlane,
        FarPlane,
        TopPlane,
        BottomPlane,
        LeftPlane,
        RightPlane,
    };

    constexpr CFrustumPlanes() = default;

    [[nodiscard]] const CPlane& GetPlane(ESide Side) const;
    void SetPlanes(const CVector3f& rkPosition, const CVector3f& rkDirection, float FieldOfView, float AspectRatio, float Near, float Far);

    [[nodiscard]] bool PointInFrustum(const CVector3f& rkPoint) const;
    [[nodiscard]] bool BoxInFrustum(const CAABox& rkBox) const;
};

#endif // AXIO_CFRUSTUMPLANES_H
