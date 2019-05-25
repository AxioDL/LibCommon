#include "CColor.h"
#include "CRandom.h"

CColor::CColor()
    : R(0.f), G(0.f), B(0.f), A(0.f)
{
}

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

CColor::CColor(float RGBA)
    : R(RGBA), G(RGBA), B(RGBA), A(RGBA)
{
}

CColor::CColor(float _R, float _G, float _B, float _A /*= 1.f*/)
    : R(_R), G(_G), B(_B), A(_A)
{
}

void CColor::SetIntegral(uint8 RGBA)
{
    float f = RGBA / 255.f;
    R = G = B = A = f;
}

void CColor::SetIntegral(uint8 _R, uint8 _G, uint8 _B, uint8 _A /*= 255*/)
{
    R = _R / 255.f;
    G = _G / 255.f;
    B = _B / 255.f;
    A = _A / 255.f;
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

bool CColor::operator==(const CColor& rkOther) const
{
    return ((R == rkOther.R) &&
            (G == rkOther.G) &&
            (B == rkOther.B) &&
            (A == rkOther.A));
}

bool CColor::operator!=(const CColor& rkOther) const
{
    return (!(*this == rkOther));
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
CColor CColor::Integral(uint8 RGBA)
{
    CColor Out;
    Out.SetIntegral(RGBA);
    return Out;
}

CColor CColor::Integral(uint8 _R, uint8 _G, uint8 _B, uint8 _A /*= 255*/)
{
    CColor Out;
    Out.SetIntegral(_R, _G, _B, _A);
    return Out;
}

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

// defining predefined colors
const CColor CColor::skRed   (1.0f, 0.0f, 0.0f);
const CColor CColor::skGreen (0.0f, 1.0f, 0.0f);
const CColor CColor::skBlue  (0.0f, 0.0f, 1.0f);
const CColor CColor::skYellow(1.0f, 1.0f, 0.0f);
const CColor CColor::skPurple(1.0f, 0.0f, 1.0f);
const CColor CColor::skCyan  (0.0f, 1.0f, 1.0f);
const CColor CColor::skWhite (1.0f, 1.0f, 1.0f);
const CColor CColor::skBlack (0.0f, 0.0f, 0.0f);
const CColor CColor::skGray  (0.5f, 0.5f, 0.5f);
const CColor CColor::skTransparentRed   (1.0f, 0.0f, 0.0f, 0.0f);
const CColor CColor::skTransparentGreen (0.0f, 1.0f, 0.0f, 0.0f);
const CColor CColor::skTransparentBlue  (0.0f, 0.0f, 1.0f, 0.0f);
const CColor CColor::skTransparentYellow(1.0f, 1.0f, 0.0f, 0.0f);
const CColor CColor::skTransparentPurple(1.0f, 0.0f, 1.0f, 0.0f);
const CColor CColor::skTransparentCyan  (0.0f, 1.0f, 1.0f, 0.0f);
const CColor CColor::skTransparentWhite (1.0f, 1.0f, 1.0f, 0.0f);
const CColor CColor::skTransparentBlack (0.0f, 0.0f, 0.0f, 0.0f);
const CColor CColor::skTransparentGray  (0.5f, 0.5f, 0.5f, 0.0f);
