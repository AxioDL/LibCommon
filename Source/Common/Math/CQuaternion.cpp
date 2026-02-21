#include "CQuaternion.h"

#include "Common/FileIO/IInputStream.h"
#include "Common/Math/CMatrix4f.h"
#include "Common/Math/CTransform4f.h"
#include "Common/Math/MathUtil.h"
#include <cmath>

CQuaternion::CQuaternion(IInputStream& rInput)
    : W(rInput.ReadF32())
    , X(rInput.ReadF32())
    , Y(rInput.ReadF32())
    , Z(rInput.ReadF32())
{
}

CQuaternion CQuaternion::Normalized() const
{
    float Norm = std::sqrt((W * W) + (X * X) + (Y * Y) + (Z * Z));
    return CQuaternion(W/Norm, X/Norm, Y/Norm, Z/Norm);
}

CQuaternion CQuaternion::Inverse() const
{
    float Norm = (W * W) + (X * X) + (Y * Y) + (Z * Z);

    if (Norm > 0.f)
    {
        float InvNorm = 1.f / Norm;
        return CQuaternion(W * InvNorm, -X * InvNorm, -Y * InvNorm, -Z * InvNorm);
    }

    return CQuaternion::Zero();
}

CQuaternion CQuaternion::Lerp(const CQuaternion& rkRight, float t) const
{
    CQuaternion Out(std::lerp(W, rkRight.W, t), std::lerp(X, rkRight.X, t),
                    std::lerp(Y, rkRight.Y, t), std::lerp(Z, rkRight.Z, t));

    return Out.Normalized();
}

CQuaternion CQuaternion::Slerp(const CQuaternion& rkRight, float t) const
{
    // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
    float CosHalfTheta = (W * rkRight.W) + (X * rkRight.X) + (Y * rkRight.Y) + (Z * rkRight.Z);

    if (std::abs(CosHalfTheta) >= 1.f)
    {
        // Fall back on lerp in this situation.
        return Lerp(rkRight, t);
    }

    float ScalarA, ScalarB;
    float SinHalfTheta = std::sqrt(1.f - (CosHalfTheta * CosHalfTheta));

    if (std::abs(SinHalfTheta) < 0.001f)
    {
        ScalarA = 0.5f;
        ScalarB = 0.5f;
    }
    else
    {
        float HalfTheta = std::acos(CosHalfTheta);
        ScalarA = std::sin((1.f - t) * HalfTheta) / SinHalfTheta;
        ScalarB = std::sin(t * HalfTheta) / SinHalfTheta;
    }

    return CQuaternion((W * ScalarA) + (rkRight.W * ScalarB),
                       (X * ScalarA) + (rkRight.X * ScalarB),
                       (Y * ScalarA) + (rkRight.Y * ScalarB),
                       (Z * ScalarA) + (rkRight.Z * ScalarB));
}

CVector3f CQuaternion::ToEuler() const
{
    // There is more than one way to do this conversion, based on rotation order.
    // But since we only care about the rotation order used in Retro games, which is consistent,
    // we can just have a single conversion function. Handy!
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

    float EulerX = std::atan2(2 * (W*X + Y*Z), 1 - (2 * (Math::Square(X) + Math::Square(Y))));
    float EulerY = std::asin(2 * (W*Y - Z*X));
    float EulerZ = std::atan2(2 * (W*Z + X*Y), 1 - (2 * (Math::Square(Y) + Math::Square(Z))));
    return CVector3f(Math::RadiansToDegrees(EulerX), Math::RadiansToDegrees(EulerY), Math::RadiansToDegrees(EulerZ));
}

// ************ OPERATORS ************
CVector3f CQuaternion::operator*(const CVector3f& rkVec) const
{
    CVector3f uv, uuv;
    CVector3f qvec(X, Y, Z);
    uv = qvec.Cross(rkVec);
    uuv = qvec.Cross(uv);
    uv *= (2.0f * W);
    uuv *= 2.0f;

    return rkVec + uv + uuv;
}

CQuaternion CQuaternion::operator*(const CQuaternion& rkOther) const
{
    CQuaternion out;
    out.W = (-X * rkOther.X) - (Y * rkOther.Y) - (Z * rkOther.Z) + (W * rkOther.W);
    out.X = ( X * rkOther.W) + (Y * rkOther.Z) - (Z * rkOther.Y) + (W * rkOther.X);
    out.Y = (-X * rkOther.Z) + (Y * rkOther.W) + (Z * rkOther.X) + (W * rkOther.Y);
    out.Z = ( X * rkOther.Y) - (Y * rkOther.X) + (Z * rkOther.W) + (W * rkOther.Z);
    return out;
}

void CQuaternion::operator *= (const CQuaternion& rkOther)
{
    *this = *this * rkOther;
}

CQuaternion CQuaternion::operator*(const CTransform4f& rkMtx) const
{
    return *this * rkMtx.ExtractRotation();
}

void CQuaternion::operator *= (const CTransform4f& rkMtx)
{
    *this = *this * rkMtx;
}

// ************ STATIC ************
CQuaternion CQuaternion::FromEuler(const CVector3f& Euler)
{
    /**
     * The commented-out code below might be faster but the conversion isn't completely correct
     * So in lieu of fixing it I'm using axis angles to convert from Eulers instead
     * I'm not sure what the difference is performance-wise but the result is 100% accurate
     */
    /*CQuaternion quat;

    // Convert from degrees to radians
    float pi = 3.14159265358979323846f;
    euler = euler * pi / 180;

    // Convert to quaternion
    float c1 = cos(euler.x / 2);
    float c2 = cos(euler.y / 2);
    float c3 = cos(euler.z / 2);
    float s1 = sin(euler.x / 2);
    float s2 = sin(euler.y / 2);
    float s3 = sin(euler.z / 2);

    quat.w =   (c1 * c2 * c3) - (s1 * s2 * s3);
    quat.x = -((s1 * c2 * c3) + (c1 * s2 * s3));
    quat.y =  ((c1 * s2 * c3) - (s1 * c2 * s3));
    quat.z =  ((s1 * s2 * c3) + (c1 * c2 * s3));*/

    CQuaternion X = CQuaternion::FromAxisAngle(Math::DegreesToRadians(Euler.X), CVector3f(1,0,0));
    CQuaternion Y = CQuaternion::FromAxisAngle(Math::DegreesToRadians(Euler.Y), CVector3f(0,1,0));
    CQuaternion Z = CQuaternion::FromAxisAngle(Math::DegreesToRadians(Euler.Z), CVector3f(0,0,1));
    CQuaternion Quat = Z * Y * X;

    return Quat;
}

CQuaternion CQuaternion::FromAxisAngle(float Angle, const CVector3f& Axis)
{
    const auto Norm = Axis.Normalized();
    const float sa = std::sin(Angle / 2);

    CQuaternion Quat;
    Quat.W = std::cos(Angle / 2);
    Quat.X = Norm.X * sa;
    Quat.Y = Norm.Y * sa;
    Quat.Z = Norm.Z * sa;
    return Quat;

}

CQuaternion CQuaternion::FromRotationMatrix(const CMatrix4f& rkRotMtx)
{
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    CQuaternion Out;
    float Trace = rkRotMtx[0][0] + rkRotMtx[1][1] + rkRotMtx[2][2];

    if (Trace > 0.f)
    {
      float s = std::sqrt(Trace + 1.f) * 2; // s = 4*w
      Out.W = 0.25f * s;
      Out.X = (rkRotMtx[2][1] - rkRotMtx[1][2]) / s;
      Out.Y = (rkRotMtx[0][2] - rkRotMtx[2][0]) / s;
      Out.Z = (rkRotMtx[1][0] - rkRotMtx[0][1]) / s;
    }
    else if ((rkRotMtx[0][0] > rkRotMtx[1][1]) && (rkRotMtx[0][0] > rkRotMtx[2][2]))
    {
      float s = std::sqrt(1.f + rkRotMtx[0][0] - rkRotMtx[1][1] - rkRotMtx[2][2]) * 2; // s = 4*x
      Out.W = (rkRotMtx[2][1] - rkRotMtx[1][2]) / s;
      Out.X = 0.25f * s;
      Out.Y = (rkRotMtx[0][1] + rkRotMtx[1][0]) / s;
      Out.Z = (rkRotMtx[0][2] + rkRotMtx[2][0]) / s;
    }
    else if (rkRotMtx[1][1] > rkRotMtx[2][2]) {
      float s = std::sqrt(1.f + rkRotMtx[1][1] - rkRotMtx[0][0] - rkRotMtx[2][2]) * 2; // s = 4*y
      Out.W = (rkRotMtx[0][2] - rkRotMtx[2][0]) / s;
      Out.X = (rkRotMtx[0][1] + rkRotMtx[1][0]) / s;
      Out.Y = 0.25f * s;
      Out.Z = (rkRotMtx[1][2] + rkRotMtx[2][1]) / s;
    }
    else {
      float s = std::sqrt(1.f + rkRotMtx[2][2] - rkRotMtx[0][0] - rkRotMtx[1][1]) * 2; // S=4*qz
      Out.W = (rkRotMtx[1][0] - rkRotMtx[0][1]) / s;
      Out.X = (rkRotMtx[0][2] + rkRotMtx[2][0]) / s;
      Out.Y = (rkRotMtx[1][2] + rkRotMtx[2][1]) / s;
      Out.Z = 0.25f * s;
    }

    return Out;
}

CQuaternion CQuaternion::FromAxes(const CVector3f& rkX, const CVector3f& rkY, const CVector3f& rkZ)
{
    CMatrix4f RotMtx(rkX.X, rkY.X, rkZ.X, 0.f,
                     rkX.Y, rkY.Y, rkZ.Y, 0.f,
                     rkX.Z, rkY.Z, rkZ.Z, 0.f,
                     0.f, 0.f, 0.f, 1.f);

    return CQuaternion::FromRotationMatrix(RotMtx);
}
