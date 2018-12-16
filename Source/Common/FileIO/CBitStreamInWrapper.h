#ifndef CBITSTREAMINWRAPPER_H
#define CBITSTREAMINWRAPPER_H

#include "IInputStream.h"

class CBitStreamInWrapper
{
public:
    enum EChunkSize
    {
        k8Bit = 8, k16Bit = 16, k32Bit = 32
    };

private:
    IInputStream *mpSourceStream;
    EChunkSize mChunkSize;
    uint32 mBitPool;
    uint32 mBitsRemaining;

public:
    CBitStreamInWrapper(IInputStream *pStream, EChunkSize ChunkSize = k32Bit);
    void SetChunkSize(EChunkSize Size);
    long ReadBits(uint32 NumBits, bool ExtendSignBit = true);
    bool ReadBit();

private:
    void ReplenishPool();
};

#endif // CBITSTREAMINWRAPPER_H
