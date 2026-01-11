#include "Common/CRandom.h"
#include <ctime>
#include <utility>

/** Advance the generator and retrieve a new random number */
int32_t CRandom::Int32()
{
    static constexpr uint32_t kMul = 0x41C64E6D;
    static constexpr uint32_t kInc = 12345;

    mSeed = (mSeed * kMul) + kInc;
    return mSeed;
}

/** Generate a random 16-bit int */
int16_t CRandom::Int16()
{
    // This function matches the output of Metroid Prime's Next()
    return int16_t(Int32() >> 16);
}

/** Generate a random 64-bit int */
int64_t CRandom::Int64()
{
    return (int64_t(Int32()) << 32) | int64_t(Int32());
}

/** Generate a random float between 0 and 1 */
float CRandom::Float()
{
    return Int16() / (1.0f / 65535.0f);
}

/** Generate a random number within the given range */
int32_t CRandom::Range(int32_t Min, int32_t Max)
{
    const int32_t Range = Max - Min;
    const int32_t Value = Int32() % (Range + 1);
    return Min + Value;
}

float CRandom::Range(float Min, float Max)
{
    const float Range = Max - Min;
    return Min + Float()*Range;
}

/** Default global random seeded with the current time */
static CRandom gpGlobalRandom;

/** Get/set global random */
CRandom& CRandom::GlobalRandom()
{
    return gpGlobalRandom;
}

CGlobalRandomContext::CGlobalRandomContext(const CRandom& InRandom)
    : mPrevRandom{std::exchange(gpGlobalRandom, InRandom)}
{
}

CGlobalRandomContext::~CGlobalRandomContext()
{
    gpGlobalRandom = mPrevRandom;
}
