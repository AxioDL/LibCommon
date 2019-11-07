#ifndef FLAGS_H
#define FLAGS_H

#include "BasicTypes.h"
#include "Serialization/IArchive.h"

template<typename FlagEnum>
class TFlags
{
    uint32 mValue;

public:
    TFlags() : mValue(0) {}
    TFlags(int Val) : mValue(Val) {}
    TFlags(uint32 Val) : mValue(Val) {}
    TFlags(FlagEnum Val) : mValue((uint32) Val) {}

    inline operator int()     const { return mValue;  }
    inline bool operator!()   const { return !mValue; }
    inline TFlags operator~() const { return TFlags(FlagEnum(~mValue)); }

    inline void operator&=(int Mask)        { mValue &= Mask; }
    inline void operator&=(uint32 Mask)     { mValue &= Mask; }
    inline void operator|=(TFlags Flags)    { mValue |= Flags.mValue; }
    inline void operator|=(FlagEnum Flag)   { mValue |= (uint32) Flag; }

    inline TFlags operator|(TFlags Flags) const     { return TFlags(FlagEnum(mValue | Flags.mValue)); }
    inline TFlags operator|(FlagEnum Flag) const    { return TFlags(FlagEnum(mValue | (uint32) Flag)); }
    inline TFlags operator&(int Mask) const         { return TFlags(FlagEnum(mValue & Mask)); }
    inline TFlags operator&(uint32 Mask) const      { return TFlags(FlagEnum(mValue & Mask)); }
    inline TFlags operator&(FlagEnum Flag) const    { return TFlags(FlagEnum(mValue & (uint32) Flag)); }

    inline bool operator==(FlagEnum Flag) const     { return mValue == (uint32) Flag; }
    inline bool operator!=(FlagEnum Flag) const     { return mValue != (uint32) Flag; }

    inline bool HasFlag(FlagEnum Flag) const    { return ((mValue & (uint32) Flag) != 0); }
    inline bool HasAnyFlags(TFlags Flags) const { return ((mValue & Flags) != 0); }
    inline bool HasAllFlags(TFlags Flags) const { return ((mValue & Flags) == Flags); }
    inline void SetFlag(FlagEnum Flag)          { mValue |= (uint32) Flag; }
    inline void SetFlag(TFlags Flags)           { mValue |= Flags; }
    inline void ClearFlag(FlagEnum Flag)        { mValue &= ~((uint32) Flag); }
    inline void ClearFlag(TFlags Flags)         { mValue &= ~Flags; }

    inline void AssignFlag(FlagEnum Flag, bool Value)
    {
        if (Value)
            SetFlag(Flag);
        else
            ClearFlag(Flag);
    }

    inline uint32 ToInt32() const               { return mValue; }
    inline void Serialize(IArchive& rArc)       { rArc.SerializePrimitive(mValue, SH_HexDisplay); }
};
// Note: When declaring flags for an enum class, use DECLARE_FLAGS_ENUMCLASS instead, defined below.
#define DECLARE_FLAGS(Enum, FlagTypeName) typedef TFlags<Enum> FlagTypeName;

// Operator definitions for enum class flags
#define ENUMCLASS_OPERATOR_OR(LeftType, RightType) \
    inline int operator|(const LeftType Left, const RightType Right) \
    { \
        return (int) Left | (int) Right; \
    }

#define ENUMCLASS_OPERATOR_AND(LeftType, RightType) \
    inline int operator&(const LeftType Left, const RightType Right) \
    { \
        return (int) Left & (int) Right; \
    }

#define ENUMCLASS_OPERATOR_BITSHIFT_LEFT(LeftType, RightType) \
    inline int operator<<(const LeftType Left, const RightType Right) \
    { \
        return (int) Left << (int) Right; \
    }

#define ENUMCLASS_OPERATOR_BITSHIFT_RIGHT(LeftType, RightType) \
    inline int operator>>(const LeftType Left, const RightType Right) \
    { \
        return (int) Left >> (int) Right; \
    }

#define ENUMCLASS_OPERATOR_NOT(InType) \
    inline int operator~(const InType Value) \
    { \
        return ~((int) Value); \
    } \

#define DECLARE_FLAGS_ENUMCLASS(Enum, FlagTypeName) \
    DECLARE_FLAGS(Enum, FlagTypeName) \
    ENUMCLASS_OPERATOR_OR(Enum, Enum) \
    ENUMCLASS_OPERATOR_OR(Enum, int) \
    ENUMCLASS_OPERATOR_OR(int, Enum) \
    ENUMCLASS_OPERATOR_AND(Enum, Enum) \
    ENUMCLASS_OPERATOR_AND(Enum, int) \
    ENUMCLASS_OPERATOR_AND(int, Enum) \
    ENUMCLASS_OPERATOR_BITSHIFT_LEFT(Enum, int) \
    ENUMCLASS_OPERATOR_BITSHIFT_RIGHT(Enum, int) \
    ENUMCLASS_OPERATOR_NOT(Enum)

#endif // FLAGS_H

