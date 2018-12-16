#ifndef CFRUSTUMPLANES_H
#define CFRUSTUMPLANES_H

#include "CAABox.h"
#include "CPlane.h"
#include "CVector3f.h"

class CFrustumPlanes
{
public:
    enum EFrustumSide
    {
        kNearPlane = 0, kFarPlane    = 1,
        kTopPlane  = 2, kBottomPlane = 3,
        kLeftPlane = 4, kRightPlane  = 5
    };

private:
    CPlane mPlanes[6];

public:
    CFrustumPlanes();
    const CPlane& GetPlane(EFrustumSide Side) const;
    void SetPlanes(const CVector3f& rkPosition, const CVector3f& rkDirection, float FieldOfView, float AspectRatio, float Near, float Far);
    bool PointInFrustum(const CVector3f& rkPoint) const;
    bool BoxInFrustum(const CAABox& rkBox) const;
};

#endif // CFRUSTUMPLANES_H
