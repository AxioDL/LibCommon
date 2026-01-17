#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <cstdint>
#include <utility>

#include <Common/Math/CVector3f.h>

class CAABox;
class CPlane;
class CRay;

namespace Math
{

// Constants
constexpr float skPi = 3.14159265358979323846f;
constexpr float skHalfPi = skPi / 2.f;

template<typename T>
constexpr T Square(const T& V)
{ return V * V; }

template<typename T>
constexpr T Cube(const T& V)
{ return V * V * V; }

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

