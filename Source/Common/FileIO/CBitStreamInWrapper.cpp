#include "CBitStreamInWrapper.h"

#include "IInputStream.h"

CBitStreamInWrapper::CBitStreamInWrapper(IInputStream *pStream, EChunkSize ChunkSize /*= e32Bit*/)
    : mpSourceStream(pStream)
    , mChunkSize(ChunkSize)
{
}

void CBitStreamInWrapper::SetChunkSize(EChunkSize Size)
{
    mChunkSize = Size;
}

int CBitStreamInWrapper::ReadBits(uint32_t NumBits, bool ExtendSignBit /*= true*/)
{
    uint32_t BitsRemaining = NumBits;
    uint32_t Out = 0;
    uint32_t Shift = 0;

    while (BitsRemaining > 0)
    {
        if (mBitsRemaining < BitsRemaining)
        {
            BitsRemaining -= mBitsRemaining;
            Out |= (mBitPool << Shift);
            Shift += mBitsRemaining;
            ReplenishPool();
        }

        else
        {
            const uint32_t Mask = (1U << BitsRemaining) - 1;
            Out |= (mBitPool & Mask) << Shift;
            mBitPool >>= BitsRemaining;
            mBitsRemaining -= BitsRemaining;
            BitsRemaining = 0;
        }
    }

    if (ExtendSignBit)
    {
        const bool Sign = ((Out >> (NumBits - 1) & 0x1) == 1);
        if (Sign)
            Out |= (UINT32_MAX << NumBits);
    }

    return static_cast<int>(Out);
}

bool CBitStreamInWrapper::ReadBit()
{
    return (ReadBits(1, false) != 0);
}

// ************ PRIVATE ************
void CBitStreamInWrapper::ReplenishPool()
{
    if (mChunkSize == k8Bit)
        mBitPool = mpSourceStream->ReadU8();
    else if (mChunkSize == k16Bit)
        mBitPool = mpSourceStream->ReadU16();
    else if (mChunkSize == k32Bit)
        mBitPool = mpSourceStream->ReadU32();

    mBitsRemaining = mChunkSize;
}
