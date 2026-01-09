#ifndef CMATRIX4_H
#define CMATRIX4_H

#include <array>
#include <cstdint>

class CQuaternion;
class CVector3f;
class CVector4f;
class CTransform4f;

class CMatrix4f
{
protected:
    union
    {
        std::array<std::array<float, 4>, 4> m;
        std::array<float, 16> _m{};
    };

public:
    CMatrix4f();
    explicit CMatrix4f(float Diagonal);
    CMatrix4f(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33);

    // Math
    CMatrix4f Transpose() const;
    CMatrix4f Inverse() const;
    float Determinant() const;

    // Operators
    std::array<float, 4>& operator[](int64_t index) { return m[index]; }
    const std::array<float, 4>& operator[](int64_t index) const { return m[index]; }
    CVector3f operator*(const CVector3f& rkVec) const;
    CVector4f operator*(const CVector4f& rkVec) const;
    CMatrix4f operator*(const CTransform4f& rkMtx) const;
    CMatrix4f operator*(const CMatrix4f& rkMtx) const;

    // Constants
    static const CMatrix4f skZero;
    static const CMatrix4f skIdentity;
};

#endif // CMATRIX4_H
