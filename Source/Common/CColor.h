#ifndef CCOLOR_H
#define CCOLOR_H

#include "BasicTypes.h"
#include "FileIO/IInputStream.h"
#include "FileIO/IOutputStream.h"
#include "Serialization/IArchive.h"

class CColor
{
public:
    float R = 0.0f;
    float G = 0.0f;
    float B = 0.0f;
    float A = 0.0f;

    constexpr CColor() = default;
    explicit CColor(IInputStream& rInput, bool Integral = false);
    constexpr CColor(float RGBA) : R{RGBA}, G{RGBA}, B{RGBA}, A{RGBA} {}
    constexpr CColor(float R_, float G_, float B_, float A_ = 1.f) : R{R_}, G{G_}, B{B_}, A{A_} {}

    constexpr void SetIntegral(uint8 RGBA) {
        const float f = RGBA / 255.f;
        R = G = B = A = f;
    }
    constexpr void SetIntegral(uint8 R_, uint8 G_, uint8 B_, uint8 A_ = 255) {
        R = R_ / 255.f;
        G = G_ / 255.f;
        B = B_ / 255.f;
        A = A_ / 255.f;
    }

    void Write(IOutputStream& rOutput, bool Integral = false) const;
    void Serialize(IArchive& rArc);

    [[nodiscard]] long ToLongRGBA() const;
    [[nodiscard]] long ToLongARGB() const;

    [[nodiscard]] constexpr bool operator==(const CColor& other) const {
        return R == other.R && G == other.G && B == other.B && A == other.A;
    }
    [[nodiscard]] constexpr bool operator!=(const CColor& other) const {
        return !operator==(other);
    }
    [[nodiscard]] CColor operator+(const CColor& rkOther) const;
    void operator+=(const CColor& rkOther);
    [[nodiscard]] CColor operator-(const CColor& rkOther) const;
    void operator-=(const CColor& rkOther);
    [[nodiscard]] CColor operator*(const CColor& rkOther) const;
    void operator*=(const CColor& rkOther);
    [[nodiscard]] CColor operator*(float Other) const;
    void operator*=(float Other);
    [[nodiscard]] CColor operator/(const CColor& rkOther) const;
    void operator/=(const CColor& rkOther);

    // Static
    [[nodiscard]] static constexpr CColor Integral(uint8 RGBA) {
        CColor out;
        out.SetIntegral(RGBA);
        return out;
    }
    [[nodiscard]] static constexpr CColor Integral(uint8 R_, uint8 G_, uint8 B_, uint8 A_ = 255) {
        CColor out;
        out.SetIntegral(R_, G_, B_, A_);
        return out;
    }
    [[nodiscard]] static CColor RandomColor(bool Transparent);
    [[nodiscard]] static CColor RandomLightColor(bool Transparent);
    [[nodiscard]] static CColor RandomDarkColor(bool Transparent);

    static constexpr CColor Red() {
        return {1.0f, 0.0f, 0.0f};
    }

    static constexpr CColor Green() {
        return {0.0f, 1.0f, 0.0f};
    }

    static constexpr CColor Blue() {
        return {0.0f, 0.0f, 1.0f};
    }

    static constexpr CColor Yellow() {
        return {1.0f, 1.0f, 0.0f};
    }

    static constexpr CColor Purple() {
        return {1.0f, 0.0f, 1.0f};
    }

    static constexpr CColor Cyan() {
        return {0.0f, 1.0f, 1.0f};
    }

    static constexpr CColor White() {
        return {1.0f, 1.0f, 1.0f};
    }

    static constexpr CColor Black() {
        return {0.0f, 0.0f, 0.0f};
    }

    static constexpr CColor Gray() {
        return {0.5f, 0.5f, 0.5f};
    }

    static constexpr CColor TransparentRed() {
        return {1.0f, 0.0f, 0.0f, 0.0f};
    }

    static constexpr CColor TransparentGreen() {
        return {0.0f, 1.0f, 0.0f, 0.0f};
    }

    static constexpr CColor TransparentBlue() {
        return {0.0f, 0.0f, 1.0f, 0.0f};
    }

    static constexpr CColor TransparentYellow() {
        return {1.0f, 1.0f, 0.0f, 0.0f};
    }

    static constexpr CColor TransparentPurple() {
        return {1.0f, 0.0f, 1.0f, 0.0f};
    }

    static constexpr CColor TransparentCyan() {
        return {0.0f, 1.0f, 1.0f, 0.0f};
    }

    static constexpr CColor TransparentWhite() {
        return {1.0f, 1.0f, 1.0f, 0.0f};
    }

    static constexpr CColor TransparentBlack() {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static constexpr CColor TransparentGray() {
        return {0.5f, 0.5f, 0.5f, 0.0f};
    }
};

#endif // CCOLOR_H
