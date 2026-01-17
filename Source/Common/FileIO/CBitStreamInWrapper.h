#ifndef AXIO_CBITSTREAMINWRAPPER_H
#define AXIO_CBITSTREAMINWRAPPER_H

#include <cstdint>

class IInputStream;

class CBitStreamInWrapper
{
public:
    enum EChunkSize
    {
        k8Bit = 8, k16Bit = 16, k32Bit = 32
    };

private:
    IInputStream *mpSourceStream = nullptr;
    EChunkSize mChunkSize{};
    uint32_t mBitPool = 0;
    uint32_t mBitsRemaining = 0;

public:
    explicit CBitStreamInWrapper(IInputStream *pStream, EChunkSize ChunkSize = k32Bit);
    void SetChunkSize(EChunkSize Size);
    long ReadBits(uint32_t NumBits, bool ExtendSignBit = true);
    bool ReadBit();

private:
    void ReplenishPool();
};

#endif // AXIO_CBITSTREAMINWRAPPER_H
