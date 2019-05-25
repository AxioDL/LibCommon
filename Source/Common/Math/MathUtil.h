#ifndef MATH_H
#define MATH_H

#include "Common/Macros.h"
#include "CAABox.h"
#include "CRay.h"
#include "CPlane.h"
#include "CVector3f.h"
#include <utility>
#include <cmath>

namespace Math
{

// Constants
static constexpr float skPi = 3.14159265358979323846f;
static constexpr float skHalfPi = skPi / 2.f;

FORCEINLINE float Abs(float Value)
{ return std::fabs(Value); }

FORCEINLINE double Abs(double Value)
{ return std::fabs(Value); }

FORCEINLINE float Pow(float Base, float Exp)
{ return std::pow(Base, Exp); }

FORCEINLINE double Pow(double Base, double Exp)
{ return std::pow(Base, Exp); }

FORCEINLINE float Sqrt(float V)
{ return std::sqrt(V); }

FORCEINLINE double Sqrt(double V)
{ return std::sqrt(V); }

template<typename T>
FORCEINLINE T Square(const T& V)
{ return V * V; }

template<typename T>
FORCEINLINE T Cube(const T& V)
{ return V * V * V; }

FORCEINLINE float Log(float V)
{ return std::log(V); }

FORCEINLINE double Log(double V)
{ return std::log(V); }

FORCEINLINE float Log2(float V)
{ return std::log2(V); }

FORCEINLINE double Log2(double V)
{ return std::log2(V); }

FORCEINLINE float Log10(float V)
{ return std::log10(V); }

FORCEINLINE double Log10(double V)
{ return std::log10(V); }

FORCEINLINE uint FloorLog2(uint V)
{
    // Binary search approach to calculating log2 of an integer
    // http://codinggorilla.domemtech.com/?p=81
    uint Out = 0;
    if (V >= 1 << 16) { Out  = 16; V >>= 16; }
    if (V >= 1 <<  8) { Out +=  8; V >>=  8; }
    if (V >= 1 <<  4) { Out +=  4; V >>=  4; }
    if (V >= 1 <<  2) { Out +=  2; V >>=  2; }
    if (V >= 1 <<  1) { Out +=  1;           }
    return Out;
}

FORCEINLINE float Distance(const CVector3f& kA, const CVector3f& kB)
{ return kA.Distance(kB); }

FORCEINLINE float DegreesToRadians(float Deg)
{
    static constexpr float Factor = skPi / 180.f;
    return Deg * Factor;
}

FORCEINLINE float RadiansToDegrees(float Rad)
{
    static constexpr float Factor = 180.f / skPi;
    return Rad * Factor;
}

template<typename T>
FORCEINLINE T Clamp(const T& kMin, const T& kMax, const T& kVal)
{
    return (kVal < kMin) ? kMin :
           (kVal > kMax) ? kMax :
                             kVal;
}

template<typename T>
FORCEINLINE T Min(const T& kA, const T& kB)
{
    return (kA < kB ? kA : kB);
}

template<typename T>
FORCEINLINE T Max(const T& kA, const T& kB)
{
    return (kA > kB ? kA : kB);
}

template<typename T>
FORCEINLINE T Lerp(const T& kA, const T& kB, float t)
{
    T Diff = kB - kA;
    return kA + T(Diff * t);
}

std::pair<bool,float> RayPlaneIntersection(const CRay& rkRay, const CPlane& rkPlane);

std::pair<bool,float> RayBoxIntersection(const CRay& rkRay, const CAABox& rkBox);

std::pair<bool,float> RayLineIntersection(const CRay& rkRay, const CVector3f& rkPointA,
                                          const CVector3f& rkPointB, float Threshold = 0.02f);

std::pair<bool,float> RaySphereIntersection(const CRay& rkRay, const CVector3f& rkSpherePos,
                                            float SphereRadius, bool AllowBackfaces = false);

std::pair<bool,float> RayTriangleIntersection(const CRay& rkRay, const CVector3f& rkPointA,
                                              const CVector3f& rkPointB, const CVector3f& rkPointC,
                                              bool AllowBackfaces = false);

CMatrix4f PerspectiveMatrix(float FOV, float Aspect, float Near, float Far);

CMatrix4f OrthographicMatrix(float Left, float Right, float Bottom, float Top, float Near, float Far);

}

#endif // MATH_H

