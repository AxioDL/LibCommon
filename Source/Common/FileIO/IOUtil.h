#ifndef IOUTIL_H
#define IOUTIL_H

#include "Common/BasicTypes.h"

namespace IOUtil
{
    enum EEndianness {
        eLittleEndian,
        eBigEndian
    };
    extern const EEndianness kSystemEndianness;

    void SwapBytes(int16& rVal);
    void SwapBytes(uint16& rVal);
    void SwapBytes(int32& rVal);
    void SwapBytes(uint32& rVal);
    void SwapBytes(int64& rVal);
    void SwapBytes(uint64& rVal);
    void SwapBytes(float& rVal);
    void SwapBytes(double& rVal);
}

#endif // IOUTIL_H
