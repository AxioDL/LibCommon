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

inline float Abs(float Value)
{ return std::fabs(Value); }

inline double Abs(double Value)
{ return std::fabs(Value); }

inline float Pow(float Base, float Exp)
{ return std::pow(Base, Exp); }

inline double Pow(double Base, double Exp)
{ return std::pow(Base, Exp); }

inline float Sqrt(float V)
{ return std::sqrt(V); }

inline double Sqrt(double V)
{ return std::sqrt(V); }

template<typename T>
constexpr T Square(const T& V)
{ return V * V; }

template<typename T>
constexpr T Cube(const T& V)
{ return V * V * V; }

inline float Log(float V)
{ return std::log(V); }

inline double Log(double V)
{ return std::log(V); }

inline float Log2(float V)
{ return std::log2(V); }

inline double Log2(double V)
{ return std::log2(V); }

inline float Log10(float V)
{ return std::log10(V); }

inline double Log10(double V)
{ return std::log10(V); }

constexpr uint FloorLog2(uint V)
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

inline float Distance(const CVector3f& kA, const CVector3f& kB)
{ return kA.Distance(kB); }

constexpr float DegreesToRadians(float Deg)
{
    constexpr float Factor = skPi / 180.f;
    return Deg * Factor;
}

constexpr float RadiansToDegrees(float Rad)
{
    constexpr float Factor = 180.f / skPi;
    return Rad * Factor;
}

template<typename T>
constexpr T Clamp(const T& kMin, const T& kMax, const T& kVal)
{
    return (kVal < kMin) ? kMin :
           (kVal > kMax) ? kMax :
                             kVal;
}

template<typename T>
constexpr T Min(const T& kA, const T& kB)
{
    return (kA < kB ? kA : kB);
}

template<typename T>
constexpr T Max(const T& kA, const T& kB)
{
    return (kA > kB ? kA : kB);
}

template<typename T>
constexpr T Lerp(const T& kA, const T& kB, float t)
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

