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
