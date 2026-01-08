#ifndef CCRC32_H
#define CCRC32_H

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

/**
 * CRC32 hash implementation
 */
class CCRC32
{
    /** Current hash value */
    uint32_t mHash = 0xFFFFFFFF;

public:
    /** Default constructor, initializes the hash to the default value */
    CCRC32();

    /** Allows the hash to be initialized to an arbitrary value */
    explicit CCRC32(uint32_t InitialValue);

    /** Hash arbitrary data */
    void Hash(const void* pkData, size_t Size);

    /** Retrieve the final output hash. (You can keep adding data to the hash after calling this.) */
    uint32_t Digest() const { return mHash; }

    /** Convenience hash methods */
    template <typename T>
    requires(std::is_arithmetic_v<T>)
    void Hash(T value) { Hash(&value, sizeof(value)); }
    void Hash(std::string_view str) { Hash(str.data(), str.size()); }

    static uint32_t StaticHashString(std::string_view str);
    static uint32_t StaticHashData(const void* pkData, size_t Size);
};

#endif // CCRC32_H
