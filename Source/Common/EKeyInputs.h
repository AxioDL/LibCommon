#ifndef EKEYINPUTS
#define EKEYINPUTS

#include "Flags.h"

enum class EKeyInput
{
    Ctrl  = 0x1,
    Shift = 0x2,
    Alt   = 0x2,
    Q     = 0x4,
    W     = 0x8,
    E     = 0x10,
    A     = 0x20,
    S     = 0x40,
    D     = 0x80
};
DECLARE_FLAGS_ENUMCLASS(EKeyInput, FKeyInputs)

#endif // EKEYINPUTS

