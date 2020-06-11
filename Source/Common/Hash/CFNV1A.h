#ifndef CFNV1A_H
#define CFNV1A_H

#include "Common/BasicTypes.h"
#include <string.h>

class CFNV1A
{
public:
    enum class EHashLength {
        k32Bit, k64Bit
    };

private:
    uint64 mHash;
    EHashLength mHashLength;

    static constexpr uint64 skFNVOffsetBasis32 = 0x811C9DC5;
    static constexpr uint64 skFNVOffsetBasis64 = 0xCBF29CE484222325;
    static constexpr uint64 skFNVPrime32 = 0x1000193;
    static constexpr uint64 skFNVPrime64 = 0x100000001B3;

public:
    explicit CFNV1A(EHashLength Length)
    {
        if (Length == EHashLength::k32Bit)
            Init32();
        else
            Init64();
    }

    void Init32()
    {
        mHashLength = EHashLength::k32Bit;
        mHash = skFNVOffsetBasis32;
    }

    void Init64()
    {
        mHashLength = EHashLength::k64Bit;
        mHash = skFNVOffsetBasis64;
    }

    void HashData(const void *pkData, uint32 Size)
    {
        const auto *pkCharData = static_cast<const char*>(pkData);
        const uint64 FNVPrime = (mHashLength == EHashLength::k32Bit) ? skFNVPrime32 : skFNVPrime64;

        for (uint32 iByte = 0; iByte < Size; iByte++)
        {
            mHash ^= *pkCharData;
            mHash *= FNVPrime;
            pkCharData++;
        }
    }

    uint32 GetHash32() const { return static_cast<uint32>(mHash); }
    uint64 GetHash64() const { return mHash; }

    // Convenience functions
    void HashByte(const uint8& rkVal)        { HashData(&rkVal, 1); }
    void HashShort(const uint16& rkVal)      { HashData(&rkVal, 2); }
    void HashLong(const uint32& rkVal)       { HashData(&rkVal, 4); }
    void HashFloat(const float& rkVal)       { HashData(&rkVal, 4); }
    void HashString(const char* pkVal)       { HashData(pkVal, strlen(pkVal)); }

    // Static
    static uint32 StaticHashData32(const void* pkData, uint Size)
    {
        CFNV1A Hasher(EHashLength::k32Bit);
        Hasher.HashData(pkData, Size);
        return Hasher.GetHash32();
    }

    static uint64 StaticHashData64(const void* pkData, uint Size)
    {
        CFNV1A Hasher(EHashLength::k64Bit);
        Hasher.HashData(pkData, Size);
        return Hasher.GetHash64();
    }
};

#endif // CFNV1A_H
