#ifndef IOUTIL_H
#define IOUTIL_H

#include "Common/BasicTypes.h"

enum class EEndian
{
    LittleEndian,
    BigEndian,
    // We only support little-endian platforms currently
    SystemEndian = EEndian::LittleEndian
};

void SwapBytes(int16& rVal);
void SwapBytes(uint16& rVal);
void SwapBytes(int32& rVal);
void SwapBytes(uint32& rVal);
void SwapBytes(int64& rVal);
void SwapBytes(uint64& rVal);
void SwapBytes(float& rVal);
void SwapBytes(double& rVal);

#endif // IOUTIL_H
