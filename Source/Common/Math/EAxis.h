#ifndef EAXIS_H
#define EAXIS_H

#include "Common/Flags.h"

enum class EAxis
{
    None = 0x0,
    X    = 0x1,
    Y    = 0x2,
    Z    = 0x4,
    XY   = X | Y,
    XZ   = X | Z,
    YZ   = Y | Z,
    XYZ  = X | Y | Z
};
DECLARE_FLAGS_ENUMCLASS(EAxis, FAxes)

#endif // EAXIS_H
