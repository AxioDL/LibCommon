#ifndef CCRC32_H
#define CCRC32_H

#include "Common/BasicTypes.h"

/**
 * CRC32 hash implementation
 */
class CCRC32
{
    /** Current hash value */
    uint32 mHash;

public:
    /** Default constructor, initializes the hash to the default value */
    CCRC32();

    /** Allows the hash to be initialized to an arbitrary value */
    CCRC32(uint32 InitialValue);

    /** Hash arbitrary data */
    void Hash(const void* pkData, int Size);

    /** Retrieve the final output hash. (You can keep adding data to the hash after calling this.) */
    uint32 Digest() const;

    /** Convenience hash methods */
    void Hash(uint8 v);
    void Hash(uint16 v);
    void Hash(uint32 v);
    void Hash(uint64 v);
    void Hash(float v);
    void Hash(double v);
    void Hash(char v);
    void Hash(const char* pkString);

    static uint32 StaticHashString(const char* pkString);
    static uint32 StaticHashData(const void* pkData, uint Size);
};

#endif // CCRC32_H
