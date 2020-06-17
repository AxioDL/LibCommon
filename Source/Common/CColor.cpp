#include "CColor.h"
#include "CRandom.h"

#include <algorithm>

CColor::CColor(IInputStream& rInput, bool Integral /*= false*/)
{
    if (Integral)
    {
        R = (uint8) rInput.ReadByte() / 255.f;
        G = (uint8) rInput.ReadByte() / 255.f;
        B = (uint8) rInput.ReadByte() / 255.f;
        A = (uint8) rInput.ReadByte() / 255.f;
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
        rOutput.WriteLong(ToLongRGBA());
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

long CColor::ToLongRGBA() const
{
    uint8 _R = (uint8) (R * 255);
    uint8 _G = (uint8) (G * 255);
    uint8 _B = (uint8) (B * 255);
    uint8 _A = (uint8) (A * 255);
    return (_R << 24) | (_G << 16) | (_B << 8) | _A;
}

long CColor::ToLongARGB() const
{
    uint8 _R = (uint8) (R * 255);
    uint8 _G = (uint8) (G * 255);
    uint8 _B = (uint8) (B * 255);
    uint8 _A = (uint8) (A * 255);
    return (_A << 24) | (_R << 16) | (_G << 8) | _B;
}

CColor CColor::operator+(const CColor& rkOther) const
{
    float NewR = std::min(R + rkOther.R, 1.f);
    float NewG = std::min(G + rkOther.G, 1.f);
    float NewB = std::min(B + rkOther.B, 1.f);
    float NewA = std::min(A + rkOther.A, 1.f);
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator+=(const CColor& rkOther)
{
    *this = (*this + rkOther);
}

CColor CColor::operator-(const CColor& rkOther) const
{
    float NewR = std::max(R - rkOther.R, 0.f);
    float NewG = std::max(G - rkOther.G, 0.f);
    float NewB = std::max(B - rkOther.B, 0.f);
    float NewA = std::max(A - rkOther.A, 0.f);
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator-=(const CColor& other)
{
    *this = (*this - other);
}

CColor CColor::operator*(const CColor& rkOther) const
{
    float NewR = R * rkOther.R;
    float NewG = G * rkOther.G;
    float NewB = B * rkOther.B;
    float NewA = A * rkOther.A;
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator*=(const CColor& rkOther)
{
    *this = (*this * rkOther);
}

CColor CColor::operator*(float Other) const
{
    float NewR = std::min( std::max(R * Other, 0.f), 1.f);
    float NewG = std::min( std::max(G * Other, 0.f), 1.f);
    float NewB = std::min( std::max(B * Other, 0.f), 1.f);
    float NewA = std::min( std::max(A * Other, 0.f), 1.f);
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator*=(float Other)
{
    *this = (*this * Other);
}

CColor CColor::operator/(const CColor& rkOther) const
{
    float NewR = (rkOther.R == 0.f) ? 0.f : R / rkOther.R;
    float NewG = (rkOther.G == 0.f) ? 0.f : G / rkOther.G;
    float NewB = (rkOther.B == 0.f) ? 0.f : B / rkOther.B;
    float NewA = (rkOther.A == 0.f) ? 0.f : A / rkOther.A;
    return CColor(NewR, NewG, NewB, NewA);
}

void CColor::operator/=(const CColor& rkOther)
{
    *this = (*this / rkOther);
}

// ************ STATIC ************

CColor CColor::RandomColor(bool Transparent)
{
    float _R = CRandom::GlobalRandom()->Float();
    float _G = CRandom::GlobalRandom()->Float();
    float _B = CRandom::GlobalRandom()->Float();
    float _A = (Transparent ? CRandom::GlobalRandom()->Float() : 1.0f);
    return CColor(_R, _G, _B, _A);
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
