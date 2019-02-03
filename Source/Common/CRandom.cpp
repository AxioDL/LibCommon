#include "CRandom.h"
#include <ctime>

/** Advance the generator and retrieve a new random number */
int32 CRandom::Int32()
{
    static const uint32 kMul = 0x41C64E6D;
    static const uint32 kInc = 12345;

    mSeed = (mSeed * kMul) + kInc;
    return mSeed;
}

/** Generate a random 16-bit int */
int16 CRandom::Int16()
{
    // This function matches the output of Metroid Prime's Next()
    return (int16) (Int32() >> 16);
}

/** Generate a random 64-bit int */
int64 CRandom::Int64()
{
    return ( ((int64) Int32()) << 32 ) | ((int64) Int32());
}

/** Generate a random float between 0 and 1 */
float CRandom::Float()
{
    return Int16() / (1.0f / 65535.0f);
}

/** Generate a random number within the given range */
int32 CRandom::Range(int32 Min, int32 Max)
{
    int32 Range = Max-Min;
    int32 Value = Int32() % (Range+1);
    return Min + Value;
}

float CRandom::Range(float Min, float Max)
{
    float Range = Max-Min;
    return Min + Float()*Range;
}

/** Default global random seeded with the current time */
CRandom* gpGlobalRandom = new CRandom;

/** Get/set global random */
CRandom* CRandom::GlobalRandom()
{
    return gpGlobalRandom;
}

CGlobalRandomContext::CGlobalRandomContext(CRandom& InRandom)
{
    mpPrevRandom = gpGlobalRandom;
    gpGlobalRandom = &InRandom;
}

CGlobalRandomContext::~CGlobalRandomContext()
{
    gpGlobalRandom = mpPrevRandom;
}
