#ifndef CVECTOR4F
#define CVECTOR4F

#include "Common/FileIO/IInputStream.h"
#include "Common/FileIO/IOutputStream.h"
#include "Common/Math/CVector2f.h"
#include "Common/Math/CVector3f.h"

class CMatrix4f;
class CTransform4f;

class CVector4f
{
public:
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float W = 0.0f;

    constexpr CVector4f() = default;
    constexpr CVector4f(float XYZW) : X{XYZW}, Y{XYZW}, Z{XYZW}, W{XYZW} {}
    constexpr CVector4f(float X_, float Y_, float Z_, float W_) : X{X_}, Y{Y_}, Z{Z_}, W{W_} {}
    constexpr CVector4f(const CVector2f& XY, float Z_, float W_) : X{XY.X}, Y{XY.Y}, Z{Z_}, W{W_} {}
    constexpr CVector4f(const CVector3f& XYZ) : X{XYZ.X}, Y{XYZ.Y}, Z{XYZ.Z}, W{1.0f} {}
    constexpr CVector4f(const CVector3f& XYZ, float W_) : X{XYZ.X}, Y{XYZ.Y}, Z{XYZ.Z}, W{W_} {}
    explicit CVector4f(IInputStream& rInput);
    void Write(IOutputStream& rOutput);

    // Swizzle
    [[nodiscard]] constexpr CVector3f XYZ() const { return {X, Y, Z}; }
    [[nodiscard]] constexpr CVector3f XZW() const { return {X, Z, W}; }
    [[nodiscard]] constexpr CVector3f YZW() const { return {Y, Z, W}; }
    [[nodiscard]] constexpr CVector2f XY() const { return {X, Y}; }
    [[nodiscard]] constexpr CVector2f XZ() const { return {X, Z}; }
    [[nodiscard]] constexpr CVector2f XW() const { return {X, W}; }
    [[nodiscard]] constexpr CVector2f YZ() const { return {Y, Z}; }
    [[nodiscard]] constexpr CVector2f YW() const { return {Y, W}; }
    [[nodiscard]] constexpr CVector2f ZW() const { return {Z, W}; }

    // Vector/Vector
    [[nodiscard]] constexpr CVector4f operator+(const CVector4f& other) const {
        return {X + other.X, Y + other.Y, Z + other.Z, W + other.W};
    }
    [[nodiscard]] constexpr CVector4f operator-(const CVector4f& other) const {
        return {X - other.X, Y - other.Y, Z - other.Z, W - other.W};
    }
    [[nodiscard]] constexpr CVector4f operator*(const CVector4f& other) const {
        return {X * other.X, Y * other.Y, Z * other.Z, W * other.W};
    }
    [[nodiscard]] constexpr CVector4f operator/(const CVector4f& other) const {
        return {X / other.X, Y / other.Y, Z / other.Z, W / other.W};
    }
    constexpr void operator+=(const CVector4f& other) {
        *this = *this + other;
    }
    constexpr void operator-=(const CVector4f& other) {
        *this = *this - other;
    }
    constexpr void operator*=(const CVector4f& other) {
        *this = *this * other;
    }
    constexpr void operator/=(const CVector4f& other) {
        *this = *this / other;
    }

    [[nodiscard]] constexpr bool operator==(const CVector4f& other) const {
        return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
    }
    [[nodiscard]] constexpr bool operator!=(const CVector4f& other) const {
        return !operator==(other);
    }

    // Vector/Float
    [[nodiscard]] constexpr CVector4f operator+(const float other) const {
        return {X + other, Y + other, Z + other, W + other};
    }
    [[nodiscard]] constexpr CVector4f operator-(const float other) const {
        return {X - other, Y - other, Z - other, W - other};
    }
    [[nodiscard]] constexpr CVector4f operator*(const float other) const {
        return {X * other, Y * other, Z * other, W * other};
    }
    [[nodiscard]] constexpr CVector4f operator/(const float other) const {
        return {X / other, Y / other, Z / other, W / other};
    }
    constexpr void operator+=(const float other) {
        *this = *this + other;
    }
    constexpr void operator-=(const float other) {
        *this = *this - other;
    }
    constexpr void operator*=(const float other) {
        *this = *this * other;
    }
    constexpr void operator/=(const float other) {
        *this = *this / other;
    }

    // Vector/Matrix
    CVector4f operator*(const CTransform4f& rkMtx) const;
    void operator*=(const CTransform4f& rkMtx);
    CVector4f operator*(const CMatrix4f& rkMtx) const;
    void operator*=(const CMatrix4f& rkMtx);

    // Unary
    [[nodiscard]] constexpr float& operator[](long Index) {
        return (&X)[Index];
    }
    [[nodiscard]] constexpr const float& operator[](long Index) const {
        return (&X)[Index];
    }
};

#endif // CVECTOR4F
