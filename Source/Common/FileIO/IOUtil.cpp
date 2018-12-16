#include "IOUtil.h"

void SwapBytes(int16& rVal)
{
    rVal = (((rVal & 0x00FF) << 8) |
           ((rVal & 0xFF00) >> 8));
}

void SwapBytes(uint16& rVal)
{
    rVal = (((rVal & 0x00FF) << 8) |
           ((rVal & 0xFF00) >> 8));
}

void SwapBytes(int32& rVal)
{
    rVal = (((rVal & 0x000000FF) << 24) |
           ((rVal & 0x0000FF00) <<  8) |
           ((rVal & 0x00FF0000) >>  8) |
           ((rVal & 0xFF000000) >> 24));
}

void SwapBytes(uint32& rVal)
{
    rVal = (((rVal & 0x000000FF) << 24) |
           ((rVal & 0x0000FF00) <<  8) |
           ((rVal & 0x00FF0000) >>  8) |
           ((rVal & 0xFF000000) >> 24));
}

void SwapBytes(int64& rVal)
{
    rVal = (((rVal & 0x00000000000000FF) << 56) |
           ((rVal & 0x000000000000FF00) << 40) |
           ((rVal & 0x0000000000FF0000) << 24) |
           ((rVal & 0x00000000FF000000) <<  8) |
           ((rVal & 0x000000FF00000000) >>  8) |
           ((rVal & 0x0000FF0000000000) >> 24) |
           ((rVal & 0x00FF000000000000) >> 40) |
           ((rVal & 0xFF00000000000000) >> 56));
}

void SwapBytes(uint64& rVal)
{
    rVal = (((rVal & 0x00000000000000FF) << 56) |
           ((rVal & 0x000000000000FF00) << 40) |
           ((rVal & 0x0000000000FF0000) << 24) |
           ((rVal & 0x00000000FF000000) <<  8) |
           ((rVal & 0x000000FF00000000) >>  8) |
           ((rVal & 0x0000FF0000000000) >> 24) |
           ((rVal & 0x00FF000000000000) >> 40) |
           ((rVal & 0xFF00000000000000) >> 56));
}

void SwapBytes(float& rVal)
{
    uint32* pPtr = (uint32*) &rVal;
    SwapBytes(*pPtr);
}

void SwapBytes(double& rVal)
{
    uint64* pPtr = (uint64*) &rVal;
    SwapBytes(*pPtr);
}
