#include "CVector4f.h"
#include "CTransform4f.h"

CVector4f::CVector4f(IInputStream& rInput)
{
    X = rInput.ReadFloat();
    Y = rInput.ReadFloat();
    Z = rInput.ReadFloat();
    W = rInput.ReadFloat();
}

void CVector4f::Write(IOutputStream& rOutput)
{
    rOutput.WriteFloat(X);
    rOutput.WriteFloat(Y);
    rOutput.WriteFloat(Z);
    rOutput.WriteFloat(W);
}

// ************ VECTOR/MATRIX ************
CVector4f CVector4f::operator*(const CTransform4f& rkMtx) const
{
    CVector4f Out;
    Out.X = (X * rkMtx[0][0]) + (Y * rkMtx[1][0]) + (Z * rkMtx[2][0]);
    Out.Y = (X * rkMtx[0][1]) + (Y * rkMtx[1][1]) + (Z * rkMtx[2][1]);
    Out.Z = (X * rkMtx[0][2]) + (Y * rkMtx[1][2]) + (Z * rkMtx[2][2]);
    Out.W = (X * rkMtx[0][3]) + (Y * rkMtx[1][3]) + (Z * rkMtx[2][3]) + W;
    return Out;
}

void CVector4f::operator*=(const CTransform4f& rkMtx)
{
    *this = *this * rkMtx;
}

CVector4f CVector4f::operator*(const CMatrix4f& rkMtx) const
{
    CVector4f Out;
    Out.X = (X * rkMtx[0][0]) + (Y * rkMtx[1][0]) + (Z * rkMtx[2][0]) + (W * rkMtx[3][0]);
    Out.Y = (X * rkMtx[0][1]) + (Y * rkMtx[1][1]) + (Z * rkMtx[2][1]) + (W * rkMtx[3][1]);
    Out.Z = (X * rkMtx[0][2]) + (Y * rkMtx[1][2]) + (Z * rkMtx[2][2]) + (W * rkMtx[3][2]);
    Out.W = (X * rkMtx[0][3]) + (Y * rkMtx[1][3]) + (Z * rkMtx[2][3]) + (W * rkMtx[3][3]);
    return Out;
}

void CVector4f::operator*=(const CMatrix4f& rkMtx)
{
    *this = *this * rkMtx;
}
