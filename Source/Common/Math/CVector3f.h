#ifndef CVECTOR3F_H
#define CVECTOR3F_H

#include "Common/FileIO/IInputStream.h"
#include "Common/FileIO/IOutputStream.h"
#include "Common/Math/CVector2f.h"
#include "Common/Serialization/IArchive.h"
#include <cmath>
#include <cfloat>
#include <ostream>

class CMatrix4f;
class CVector4f;
class CTransform4f;

class CVector3f
{
public:
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    constexpr CVector3f() = default;
    constexpr CVector3f(float XYZ) : X{XYZ}, Y{XYZ}, Z{XYZ} {}
    constexpr CVector3f(float X_, float Y_, float Z_) : X{X_}, Y{Y_}, Z{Z_} {}
    explicit CVector3f(IInputStream& Input);
    void Read(IInputStream& Input);
    void Write(IOutputStream& Output) const;
    void Serialize(IArchive& Arc);
    TString ToString() const;

    // Swizzle
    [[nodiscard]] constexpr CVector2f XY() const { return {X, Y}; }
    [[nodiscard]] constexpr CVector2f XZ() const { return {X, Z}; }
    [[nodiscard]] constexpr CVector2f YZ() const { return {Y, Z}; }

    // Math
    [[nodiscard]] float Magnitude() const {
        return std::sqrt(SquaredMagnitude());
    }
    [[nodiscard]] float SquaredMagnitude() const {
        return Dot(*this);
    }
    [[nodiscard]] CVector3f Normalized() const {
        return *this / Magnitude();
    }
    [[nodiscard]] constexpr float Dot(const CVector3f& other) const {
        return (X * other.X) + (Y * other.Y) + (Z * other.Z);
    }
    [[nodiscard]] constexpr CVector3f Cross(const CVector3f& other) const {
        return {(Y * other.Z) - (Z * other.Y), (Z * other.X) - (X * other.Z), (X * other.Y) - (Y * other.X)};
    }
    [[nodiscard]] float Distance(const CVector3f& point) const {
        return (*this - point).Magnitude();
    }
    [[nodiscard]] float SquaredDistance(const CVector3f& point) const {
        return (*this - point).SquaredMagnitude();
    }

    // Vector/Vector
    [[nodiscard]] constexpr CVector3f operator+(const CVector3f& other) const {
        return {X + other.X, Y + other.Y, Z + other.Z};
    }
    [[nodiscard]] constexpr CVector3f operator-(const CVector3f& other) const {
        return {X - other.X, Y - other.Y, Z - other.Z};
    }
    [[nodiscard]] constexpr CVector3f operator*(const CVector3f& other) const {
        return {X * other.X, Y * other.Y, Z * other.Z};
    }
    [[nodiscard]] constexpr CVector3f operator/(const CVector3f& other) const {
        return {X / other.X, Y / other.Y, Z / other.Z};
    }
    constexpr void operator+=(const CVector3f& other) {
        *this = *this + other;
    }
    constexpr void operator-=(const CVector3f& other) {
        *this = *this - other;
    }
    constexpr void operator*=(const CVector3f& other) {
        *this = *this * other;
    }
    constexpr void operator/=(const CVector3f& other) {
        *this = *this / other;
    }
    [[nodiscard]] constexpr bool operator> (const CVector3f& other) const {
        return X > other.X && Y > other.Y && Z > other.Z;
    }
    [[nodiscard]] constexpr bool operator>=(const CVector3f& other) const {
        return X >= other.X && Y >= other.Y && Z >= other.Z;
    }
    [[nodiscard]] constexpr bool operator< (const CVector3f& other) const {
        return X < other.X && Y < other.Y && Z < other.Z;
    }
    [[nodiscard]] constexpr bool operator<=(const CVector3f& other) const {
        return X <= other.X && Y <= other.Y && Z <= other.Z;
    }
    [[nodiscard]] constexpr bool operator==(const CVector3f& other) const {
        return X == other.X && Y == other.Y && Z == other.Z;
    }
    [[nodiscard]] constexpr bool operator!=(const CVector3f& other) const {
        return !operator==(other);
    }

    // Vector/Float
    [[nodiscard]] constexpr CVector3f operator+(const float other) const {
        return {X + other, Y + other, Z + other};
    }
    [[nodiscard]] constexpr CVector3f operator-(const float other) const {
        return {X - other, Y - other, Z - other};
    }
    [[nodiscard]] constexpr CVector3f operator*(const float other) const {
        return {X * other, Y * other, Z * other};
    }
    [[nodiscard]] constexpr CVector3f operator/(const float other) const {
        return {X / other, Y / other, Z / other};
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
    CVector3f operator*(const CTransform4f& rkMtx) const;
    void operator*=(const CTransform4f& rkMtx);
    CVector3f operator*(const CMatrix4f& rkMtx) const;

    // Unary
    [[nodiscard]] constexpr CVector3f operator-() const {
        return {-X, -Y, -Z};
    }
    [[nodiscard]] constexpr float& operator[](long Index) {
        return (&X)[Index];
    }
    [[nodiscard]] constexpr const float& operator[](long Index) const {
        return (&X)[Index];
    }

    static constexpr CVector3f Zero() {
        return {0.0f};
    }

    static constexpr CVector3f One() {
        return {1.0f};
    }

    static constexpr CVector3f Infinite() {
        return {FLT_MAX};
    }

    static constexpr CVector3f UnitX() {
        return {1.f, 0.f, 0.f};
    }

    static constexpr CVector3f UnitY() {
        return {0.f, 1.f, 0.f};
    }

    static constexpr CVector3f UnitZ() {
        return {0.f, 0.f, 1.f};
    }

    static constexpr CVector3f Right() {
        return UnitX();
    }

    static constexpr CVector3f Left() {
        return -UnitX();
    }

    static constexpr CVector3f Forward() {
        return UnitY();
    }

    static constexpr CVector3f Back() {
        return -UnitY();
    }

    static constexpr CVector3f Up() {
        return UnitZ();
    }

    static constexpr CVector3f Down() {
        return -UnitZ();
    }

    // ************ CONSTANTS ************
    static const CVector3f skUnitX;
    static const CVector3f skUnitY;
    static const CVector3f skUnitZ;
    static const CVector3f skRight;
    static const CVector3f skLeft;
    static const CVector3f skForward;
    static const CVector3f skBack;
    static const CVector3f skUp;
    static const CVector3f skDown;

    // Other
    friend std::ostream& operator<<(std::ostream& rOut, const CVector3f& rkVector);
};

#endif // CVECTOR3F_H
