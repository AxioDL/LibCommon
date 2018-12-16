#ifndef CFNV1A_H
#define CFNV1A_H

#include "Common/BasicTypes.h"
#include "Common/TString.h"

class CFNV1A
{
public:
    enum EHashLength {
        k32Bit, k64Bit
    };

private:
    uint64 mHash;
    EHashLength mHashLength;

    static const uint64 skFNVOffsetBasis32 = 0x811C9DC5;
    static const uint64 skFNVOffsetBasis64 = 0xCBF29CE484222325;
    static const uint64 skFNVPrime32 = 0x1000193;
    static const uint64 skFNVPrime64 = 0x100000001B3;

public:
    CFNV1A(EHashLength Length)
    {
        if (Length == k32Bit)
            Init32();
        else
            Init64();
    }

    void Init32()
    {
        mHashLength = k32Bit;
        mHash = skFNVOffsetBasis32;
    }

    void Init64()
    {
        mHashLength = k64Bit;
        mHash = skFNVOffsetBasis64;
    }

    void HashData(const void *pkData, uint32 Size)
    {
        const char *pkCharData = (const char*) pkData;
        uint64 FNVPrime = (mHashLength == k32Bit) ? skFNVPrime32 : skFNVPrime64;

        for (uint32 iByte = 0; iByte < Size; iByte++)
        {
            mHash ^= *pkCharData;
            mHash *= FNVPrime;
            pkCharData++;
        }
    }

    inline uint32 GetHash32()  { return (uint32) mHash; }
    inline uint64 GetHash64()  { return mHash; }

    // Convenience functions
    inline void HashByte(const uint8& rkVal)        { HashData(&rkVal, 1); }
    inline void HashShort(const uint16& rkVal)      { HashData(&rkVal, 2); }
    inline void HashLong(const uint32& rkVal)       { HashData(&rkVal, 4); }
    inline void HashFloat(const float& rkVal)       { HashData(&rkVal, 4); }
    inline void HashString(const TString& rkVal)    { HashData(rkVal.Data(), rkVal.Size()); }
};

#endif // CFNV1A_H
