#ifndef AXIO_CFNV1A_H
#define AXIO_CFNV1A_H

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

class CFNV1A
{
public:
    enum class EHashLength {
        k32Bit, k64Bit
    };

private:
    uint64_t mHash;
    EHashLength mHashLength;

    static constexpr uint64_t skFNVOffsetBasis32 = 0x811C9DC5;
    static constexpr uint64_t skFNVOffsetBasis64 = 0xCBF29CE484222325;
    static constexpr uint64_t skFNVPrime32 = 0x1000193;
    static constexpr uint64_t skFNVPrime64 = 0x100000001B3;

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

    uint32_t GetHash32() const { return static_cast<uint32_t>(mHash); }
    uint64_t GetHash64() const { return mHash; }

    void HashData(const void *pkData, size_t Size)
    {
        const auto* pkCharData = static_cast<const char*>(pkData);
        const auto FNVPrime = (mHashLength == EHashLength::k32Bit) ? skFNVPrime32 : skFNVPrime64;

        for (size_t iByte = 0; iByte < Size; iByte++)
        {
            mHash ^= *pkCharData;
            mHash *= FNVPrime;
            pkCharData++;
        }
    }

    // Convenience functions
    void HashData(std::string_view str)
    {
        HashData(str.data(), str.size());
    }
    template <typename T>
    requires(std::is_trivially_copyable_v<T>)
    void HashData(T data)
    {
        HashData(&data, sizeof(data));
    }

    // Static
    static uint32_t StaticHashData32(const void* pkData, size_t Size)
    {
        CFNV1A Hasher(EHashLength::k32Bit);
        Hasher.HashData(pkData, Size);
        return Hasher.GetHash32();
    }

    static uint64_t StaticHashData64(const void* pkData, size_t Size)
    {
        CFNV1A Hasher(EHashLength::k64Bit);
        Hasher.HashData(pkData, Size);
        return Hasher.GetHash64();
    }
};

#endif // AXIO_CFNV1A_H
