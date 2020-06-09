#include "CVector2f.h"

CVector2f::CVector2f(IInputStream& rInput)
{
    X = rInput.ReadFloat();
    Y = rInput.ReadFloat();
}

void CVector2f::Write(IOutputStream& rOutput) const
{
    rOutput.WriteFloat(X);
    rOutput.WriteFloat(Y);
}


// ************ STATIC MEMBER INITIALIZATION ************
const CVector2f CVector2f::skZero  = CVector2f(0, 0);
const CVector2f CVector2f::skOne   = CVector2f(1, 1);
const CVector2f CVector2f::skUp    = CVector2f(0, 1);
const CVector2f CVector2f::skRight = CVector2f(1, 0);
const CVector2f CVector2f::skDown  = CVector2f(0,-1);
const CVector2f CVector2f::skLeft  = CVector2f(-1,0);
