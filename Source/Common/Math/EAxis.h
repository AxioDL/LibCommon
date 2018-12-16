#ifndef EAXIS_H
#define EAXIS_H

#include "Common/Flags.h"

enum class EAxis
{
    None = 0x0,
    X    = 0x1,
    Y    = 0x2,
    Z    = 0x4,
    XY   = EAxis::X | EAxis::Y,
    XZ   = EAxis::X | EAxis::Z,
    YZ   = EAxis::Y | EAxis::Z,
    XYZ  = EAxis::X | EAxis::Y | EAxis::Z
};
DECLARE_FLAGS_ENUMCLASS(EAxis, FAxes)

#endif // EAXIS_H
