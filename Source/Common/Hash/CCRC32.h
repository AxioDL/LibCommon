#ifndef AXIO_CCRC32_H
#define AXIO_CCRC32_H

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
    void Hash(const char* str) { Hash(std::string_view(str)); }
    void Hash(std::string_view str) { Hash(str.data(), str.size()); }

    template <typename T>
    requires(std::is_trivially_copyable_v<T> && !std::is_pointer_v<T>)
    void Hash(const T& value) { Hash(&value, sizeof(value)); }

    static uint32_t StaticHashString(std::string_view str);
    static uint32_t StaticHashData(const void* pkData, size_t Size);
};

#endif // AXIO_CCRC32_H
