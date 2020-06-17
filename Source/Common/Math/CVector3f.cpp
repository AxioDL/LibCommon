#include "CVector3f.h"
#include "CVector2f.h"
#include "CVector4f.h"
#include "CTransform4f.h"
#include <iomanip>
#include <float.h>
#include <cmath>

CVector3f::CVector3f(IInputStream& Input)
{
    Read(Input);
}

void CVector3f::Read(IInputStream& Input)
{
    X = Input.ReadFloat();
    Y = Input.ReadFloat();
    Z = Input.ReadFloat();
}

void CVector3f::Write(IOutputStream& Output) const
{
    Output.WriteFloat(X);
    Output.WriteFloat(Y);
    Output.WriteFloat(Z);
}

void CVector3f::Serialize(IArchive& Arc)
{
    Arc << SerialParameter("X", X)
        << SerialParameter("Y", Y)
        << SerialParameter("Z", Z);
}

TString CVector3f::ToString() const
{
    return TString::Format("%s, %s, %s",
                           *TString::FromFloat(X),
                           *TString::FromFloat(Y),
                           *TString::FromFloat(Z));
}

// ************ OPERATORS ************

// VECTOR/MATRIX
CVector3f CVector3f::operator*(const CTransform4f& rkMtx) const
{
    CVector3f Out;
    Out.X = (X * rkMtx[0][0]) + (Y * rkMtx[1][0]) + (Z * rkMtx[2][0]);
    Out.Y = (X * rkMtx[0][1]) + (Y * rkMtx[1][1]) + (Z * rkMtx[2][1]);
    Out.Z = (X * rkMtx[0][2]) + (Y * rkMtx[1][2]) + (Z * rkMtx[2][2]);
    return Out;
}

void CVector3f::operator*=(const CTransform4f& rkMtx)
{
    *this = *this * rkMtx;
}

CVector3f CVector3f::operator*(const CMatrix4f& rkMtx) const
{
    // To multiply by a Matrix4f, we consider the vector w component to be 1
    CVector3f Out;
    float W = (X * rkMtx[0][3]) + (Y * rkMtx[1][3]) + (Z * rkMtx[2][3]) + rkMtx[3][3];
    Out.X = ((X * rkMtx[0][0]) + (Y * rkMtx[1][0]) + (Z * rkMtx[2][0]) + rkMtx[3][0]) / W;
    Out.Y = ((X * rkMtx[0][1]) + (Y * rkMtx[1][1]) + (Z * rkMtx[2][1]) + rkMtx[3][1]) / W;
    Out.Z = ((X * rkMtx[0][2]) + (Y * rkMtx[1][2]) + (Z * rkMtx[2][2]) + rkMtx[3][2]) / W;
    return Out;
}

// ************ CONSTANTS ************
const CVector3f CVector3f::skUnitX    = CVector3f(1.f, 0.f, 0.f);
const CVector3f CVector3f::skUnitY    = CVector3f(0.f, 1.f, 0.f);
const CVector3f CVector3f::skUnitZ    = CVector3f(0.f, 0.f, 1.f);
const CVector3f CVector3f::skRight    = CVector3f::skUnitX;
const CVector3f CVector3f::skLeft     = -CVector3f::skUnitX;
const CVector3f CVector3f::skForward  = CVector3f::skUnitY;
const CVector3f CVector3f::skBack     = -CVector3f::skUnitY;
const CVector3f CVector3f::skUp       = CVector3f::skUnitZ;
const CVector3f CVector3f::skDown     = -CVector3f::skUnitZ;

// ************ OTHER ************
std::ostream& operator<<(std::ostream& rOut, const CVector3f& rkVector)
{
    rOut << std::setprecision(6)
      << std::fixed
      << "["
      << ((rkVector.X >= 0) ? " " : "")
      << rkVector.X
      << ", "
      << ((rkVector.Y >= 0) ? " " : "")
      << rkVector.Y
      << ", "
      << ((rkVector.Z >= 0) ? " " : "")
      << rkVector.Z
      << "]";

    return rOut;
}
