#ifndef CVECTOR2I_H
#define CVECTOR2I_H

class CVector2i
{
public:
    int X = 0;
    int Y = 0;

    constexpr CVector2i() = default;
    constexpr CVector2i(int XY) : X{XY}, Y{XY} {}
    constexpr CVector2i(int X_, int Y_) : X{X_}, Y{Y_} {}

    [[nodiscard]] constexpr CVector2i operator+(const CVector2i& other) const {
        return {X + other.X, Y + other.Y};
    }
    [[nodiscard]] constexpr CVector2i operator-(const CVector2i& other) const {
        return {X - other.X, Y - other.Y};
    }
    [[nodiscard]] constexpr CVector2i operator*(const CVector2i& other) const {
        return {X * other.X, Y * other.Y};
    }
    [[nodiscard]] constexpr CVector2i operator/(const CVector2i& other) const {
        return {X / other.X, Y / other.Y};
    }

    constexpr void operator+=(const CVector2i& other) {
        *this = *this + other;
    }
    constexpr void operator-=(const CVector2i& other) {
        *this = *this - other;
    }
    constexpr void operator*=(const CVector2i& other) {
        *this = *this * other;
    }
    constexpr void operator/=(const CVector2i& other) {
        *this = *this / other;
    }

    [[nodiscard]] constexpr CVector2i operator+(const int other) const {
        return {X + other, Y + other};
    }
    [[nodiscard]] constexpr CVector2i operator-(const int other) const {
        return {X - other, Y - other};
    }
    [[nodiscard]] constexpr CVector2i operator*(const int other) const {
        return {X * other, Y * other};
    }
    [[nodiscard]] constexpr CVector2i operator/(const int other) const {
        return {X / other, Y / other};
    }

    constexpr void operator+=(const int other) {
        *this = *this + other;
    }
    constexpr void operator-=(const int other) {
        *this = *this - other;
    }
    constexpr void operator*=(const int other) {
        *this = *this * other;
    }
    constexpr void operator/=(const int other) {
        *this = *this / other;
    }

    [[nodiscard]] constexpr bool operator==(const CVector2i& other) const {
        return X == other.X && Y == other.Y;
    }
    [[nodiscard]] constexpr bool operator!=(const CVector2i& other) const {
        return !operator==(other);
    }

    [[nodiscard]] constexpr int& operator[](int index) {
        return (&X)[index];
    }

    [[nodiscard]] constexpr const int& operator[](int index) const {
        return (&X)[index];
    }

    // Static members
    static constexpr CVector2i Zero() {
        return {0};
    }
};

#endif // CVECTOR2I_H
