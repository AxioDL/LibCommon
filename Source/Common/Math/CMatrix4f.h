#ifndef AXIO_CMATRIX4_H
#define AXIO_CMATRIX4_H

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
    constexpr CMatrix4f() = default;

    constexpr explicit CMatrix4f(float Diagonal)
    {
        m = {};
        m[0][0] = Diagonal;
        m[1][1] = Diagonal;
        m[2][2] = Diagonal;
        m[3][3] = Diagonal;
    }

    constexpr CMatrix4f(float m00, float m01, float m02, float m03,
                        float m10, float m11, float m12, float m13,
                        float m20, float m21, float m22, float m23,
                        float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;
        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    // Math
    CMatrix4f Transpose() const;
    CMatrix4f Inverse() const;
    float Determinant() const;

    // Operators
    constexpr std::array<float, 4>& operator[](int64_t index) { return m[index]; }
    constexpr const std::array<float, 4>& operator[](int64_t index) const { return m[index]; }
    CVector3f operator*(const CVector3f& rkVec) const;
    CVector4f operator*(const CVector4f& rkVec) const;
    CMatrix4f operator*(const CTransform4f& rkMtx) const;
    CMatrix4f operator*(const CMatrix4f& rkMtx) const;

    // Constants
    static const CMatrix4f skZero;
    static const CMatrix4f skIdentity;
};

#endif // AXIO_CMATRIX4_H
