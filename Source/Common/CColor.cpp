#include "CColor.h"

#include "Common/CRandom.h"
#include "Common/FileIO/IInputStream.h"
#include "Common/FileIO/IOutputStream.h"
#include "Common/Serialization/IArchive.h"

#include <algorithm>

CColor::CColor(IInputStream& rInput, bool Integral /*= false*/)
{
    if (Integral)
    {
        R = (uint8_t) rInput.ReadByte() / 255.f;
        G = (uint8_t) rInput.ReadByte() / 255.f;
        B = (uint8_t) rInput.ReadByte() / 255.f;
        A = (uint8_t) rInput.ReadByte() / 255.f;
    }
    else
    {
        R = rInput.ReadFloat();
        G = rInput.ReadFloat();
        B = rInput.ReadFloat();
        A = rInput.ReadFloat();
    }
}

void CColor::Write(IOutputStream &rOutput, bool Integral /*= false*/) const
{
    if (Integral)
    {
        rOutput.WriteULong(ToRGBA());
    }
    else
    {
        rOutput.WriteFloat(R);
        rOutput.WriteFloat(G);
        rOutput.WriteFloat(B);
        rOutput.WriteFloat(A);
    }
}

void CColor::Serialize(IArchive& rArc)
{
    rArc << SerialParameter("R", R)
         << SerialParameter("G", G)
         << SerialParameter("B", B)
         << SerialParameter("A", A, SH_Optional, 1.0f);
}

uint32_t CColor::ToRGBA() const
{
    const uint32_t r = uint8_t(R * 255);
    const uint32_t g = uint8_t(G * 255);
    const uint32_t b = uint8_t(B * 255);
    const uint32_t a = uint8_t(A * 255);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

uint32_t CColor::ToARGB() const
{
    const uint32_t r = uint8_t(R * 255);
    const uint32_t g = uint8_t(G * 255);
    const uint32_t b = uint8_t(B * 255);
    const uint32_t a = uint8_t(A * 255);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

CColor CColor::operator+(const CColor& rkOther) const
{
    const auto NewR = std::min(R + rkOther.R, 1.f);
    const auto NewG = std::min(G + rkOther.G, 1.f);
    const auto NewB = std::min(B + rkOther.B, 1.f);
    const auto NewA = std::min(A + rkOther.A, 1.f);
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator+=(const CColor& rkOther)
{
    *this = (*this + rkOther);
}

CColor CColor::operator-(const CColor& rkOther) const
{
    const auto NewR = std::max(R - rkOther.R, 0.f);
    const auto NewG = std::max(G - rkOther.G, 0.f);
    const auto NewB = std::max(B - rkOther.B, 0.f);
    const auto NewA = std::max(A - rkOther.A, 0.f);
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator-=(const CColor& other)
{
    *this = (*this - other);
}

CColor CColor::operator*(const CColor& rkOther) const
{
    const auto NewR = R * rkOther.R;
    const auto NewG = G * rkOther.G;
    const auto NewB = B * rkOther.B;
    const auto NewA = A * rkOther.A;
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator*=(const CColor& rkOther)
{
    *this = (*this * rkOther);
}

CColor CColor::operator*(float Other) const
{
    const auto NewR = std::clamp(R * Other, 0.f, 1.f);
    const auto NewG = std::clamp(G * Other, 0.f, 1.f);
    const auto NewB = std::clamp(B * Other, 0.f, 1.f);
    const auto NewA = std::clamp(A * Other, 0.f, 1.f);
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator*=(float Other)
{
    *this = (*this * Other);
}

CColor CColor::operator/(const CColor& rkOther) const
{
    const auto NewR = (rkOther.R == 0.f) ? 0.f : R / rkOther.R;
    const auto NewG = (rkOther.G == 0.f) ? 0.f : G / rkOther.G;
    const auto NewB = (rkOther.B == 0.f) ? 0.f : B / rkOther.B;
    const auto NewA = (rkOther.A == 0.f) ? 0.f : A / rkOther.A;
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator/=(const CColor& rkOther)
{
    *this = (*this / rkOther);
}

// ************ STATIC ************

CColor CColor::RandomColor(bool Transparent)
{
    const auto r = CRandom::GlobalRandom()->Float();
    const auto g = CRandom::GlobalRandom()->Float();
    const auto b = CRandom::GlobalRandom()->Float();
    const auto a = (Transparent ? CRandom::GlobalRandom()->Float() : 1.0f);
    return CColor(r, g, b, a);
}

CColor CColor::RandomLightColor(bool Transparent)
{
    return (RandomColor(Transparent) * 0.5f) +
            CColor(0.5f, 0.5f, 0.5f, Transparent ? 0.5f : 0.0f);
}

CColor CColor::RandomDarkColor(bool Transparent)
{
    return RandomColor(Transparent) * 0.5f;
}
