#ifndef EMOUSEINPUTS
#define EMOUSEINPUTS

#include "Flags.h"

enum class EMouseInput
{
    LeftButton   = 0x1,
    MiddleButton = 0x2,
    RightButton  = 0x4
};
DECLARE_FLAGS_ENUMCLASS(EMouseInput, FMouseInputs)

#endif // EMOUSEINPUTS

