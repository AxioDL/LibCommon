#ifndef CQUATERNION_H
#define CQUATERNION_H

#include "CVector3f.h"

class CQuaternion
{
public:
    float W = 1.0f;
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    constexpr CQuaternion() = default;
    constexpr CQuaternion(float W_, float X_, float Y_, float Z_) : W{W_}, X{X_}, Y{Y_}, Z{Z_} {}
    explicit CQuaternion(IInputStream& rInput);

    [[nodiscard]] CVector3f XAxis() const {
        return *this * CVector3f::UnitX();
    }
    [[nodiscard]] CVector3f YAxis() const {
        return *this * CVector3f::UnitY();
    }
    [[nodiscard]] CVector3f ZAxis() const {
        return *this * CVector3f::UnitZ();
    }
    [[nodiscard]] CQuaternion Normalized() const;
    [[nodiscard]] CQuaternion Inverse() const;
    [[nodiscard]] CQuaternion Lerp(const CQuaternion& rkRight, float t) const;
    [[nodiscard]] CQuaternion Slerp(const CQuaternion& rkRight, float t) const;
    [[nodiscard]] CVector3f ToEuler() const;

    // Operators
    [[nodiscard]] CVector3f operator*(const CVector3f& rkVec) const;
    [[nodiscard]] CQuaternion operator*(const CQuaternion& rkOther) const;
    void operator *= (const CQuaternion& rkOther);
    [[nodiscard]] CQuaternion operator*(const CTransform4f& rkMtx) const;
    void operator *= (const CTransform4f& rkMtx);

    // Static
    [[nodiscard]] static CQuaternion FromEuler(CVector3f Euler);
    [[nodiscard]] static CQuaternion FromAxisAngle(float Angle, CVector3f Axis);
    [[nodiscard]] static CQuaternion FromRotationMatrix(const CMatrix4f& rkRotMtx);
    [[nodiscard]] static CQuaternion FromAxes(const CVector3f& rkX, const CVector3f& rkY, const CVector3f& rkZ);

    static constexpr CQuaternion Identity() {
        return {1.0f, 0.0f, 0.0f, 0.0f};
    }

    static constexpr CQuaternion Zero() {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static CQuaternion skIdentity;
    static CQuaternion skZero;
};

#endif // CQUATERNION_H
