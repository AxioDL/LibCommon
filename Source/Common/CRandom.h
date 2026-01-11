#ifndef CRANDOM_H
#define CRANDOM_H

#include <cstdint>
#include <ctime>

/** CRandom - LCG random number generator implementation */
class CRandom
{
    /** Current random seed */
    uint32_t mSeed;

public:
    /** Constructor with the seed default-initialized to the current time */
    explicit CRandom(uint32_t InSeed = (uint32_t) time(NULL))
        : mSeed(InSeed)
    {}

    /** Advance the generator and retrieve a new random number */
    int32_t Int32();

    /** Generate a random 16-bit int */
    int16_t Int16();

    /** Generate a random 64-bit int */
    int64_t Int64();

    /** Generate a random float between 0 and 1 */
    float Float();

    /** Generate a random number within the given range */
    int32_t Range(int32_t Min, int32_t Max);
    float Range(float Min, float Max);

    /** Get/set current seed */
    inline uint32_t GetSeed() const       { return mSeed; }
    inline void SetSeed(uint32_t NewSeed) { mSeed = NewSeed; }

    /** We track a single "active" CRandom instance globally. This is essentially
     *  a drop-in replacement for rand() but with the option of being able to
     *  use a deterministic RNG for certain scenarios by changing the global random.
     */
    static CRandom* GlobalRandom();
};

/** CGlobalRandomContext - Push a CRandom instance as the global random */
class CGlobalRandomContext
{
    /** The previous global CRandom instance */
    CRandom* mpPrevRandom;

public:
    explicit CGlobalRandomContext(CRandom& InRandom);
    ~CGlobalRandomContext();
};

#endif // CRANDOM_H
