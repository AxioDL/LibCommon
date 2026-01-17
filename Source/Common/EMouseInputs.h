#ifndef AXIO_EMOUSEINPUTS_H
#define AXIO_EMOUSEINPUTS_H

#include "Flags.h"

enum class EMouseInput
{
    LeftButton   = 0x1,
    MiddleButton = 0x2,
    RightButton  = 0x4
};
AXIO_DECLARE_FLAGS_ENUMCLASS(EMouseInput, FMouseInputs)

#endif // AXIO_EMOUSEINPUTS_H

