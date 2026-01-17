#ifndef AXIO_CFRUSTUMPLANES_H
#define AXIO_CFRUSTUMPLANES_H

#include "CPlane.h"
#include <array>

class CAABox;
class CVector3f;

class CFrustumPlanes
{
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

private:
    std::array<CPlane, 6> mPlanes;

public:
    CFrustumPlanes();
    const CPlane& GetPlane(ESide Side) const;
    void SetPlanes(const CVector3f& rkPosition, const CVector3f& rkDirection, float FieldOfView, float AspectRatio, float Near, float Far);
    bool PointInFrustum(const CVector3f& rkPoint) const;
    bool BoxInFrustum(const CAABox& rkBox) const;
};

#endif // AXIO_CFRUSTUMPLANES_H
