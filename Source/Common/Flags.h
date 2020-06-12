#ifndef FLAGS_H
#define FLAGS_H

#include "BasicTypes.h"
#include "Serialization/IArchive.h"

template<typename FlagEnum>
class TFlags
{
    uint32 mValue = 0;

public:
    constexpr TFlags() = default;
    constexpr TFlags(int Val) : mValue(uint32(Val)) {}
    constexpr TFlags(uint32 Val) : mValue(Val) {}
    constexpr TFlags(FlagEnum Val) : mValue(uint32(Val)) {}

    constexpr operator int()     const { return int(mValue);  }
    [[nodiscard]] constexpr bool operator!()   const { return !mValue; }
    [[nodiscard]] constexpr TFlags operator~() const { return TFlags(FlagEnum(~mValue)); }

    constexpr void operator&=(int Mask)        { mValue &= uint32(Mask); }
    constexpr void operator&=(uint32 Mask)     { mValue &= Mask; }
    constexpr void operator|=(TFlags Flags)    { mValue |= Flags.mValue; }
    constexpr void operator|=(FlagEnum Flag)   { mValue |= uint32(Flag); }

    [[nodiscard]] constexpr TFlags operator|(TFlags Flags) const     { return TFlags(FlagEnum(mValue | Flags.mValue)); }
    [[nodiscard]] constexpr TFlags operator|(FlagEnum Flag) const    { return TFlags(FlagEnum(mValue | uint32(Flag))); }
    [[nodiscard]] constexpr TFlags operator&(int Mask) const         { return TFlags(FlagEnum(mValue & uint32(Mask))); }
    [[nodiscard]] constexpr TFlags operator&(uint32 Mask) const      { return TFlags(FlagEnum(mValue & Mask)); }
    [[nodiscard]] constexpr TFlags operator&(FlagEnum Flag) const    { return TFlags(FlagEnum(mValue & uint32(Flag))); }

    [[nodiscard]] constexpr bool operator==(FlagEnum Flag) const     { return mValue == uint32(Flag); }
    [[nodiscard]] constexpr bool operator!=(FlagEnum Flag) const     { return !operator==(Flag); }

    [[nodiscard]] constexpr bool HasFlag(FlagEnum Flag) const    { return ((mValue & (uint32) Flag) != 0); }
    [[nodiscard]] constexpr bool HasAnyFlags(TFlags Flags) const { return ((mValue & Flags) != 0); }
    [[nodiscard]] constexpr bool HasAllFlags(TFlags Flags) const { return ((mValue & Flags) == Flags); }
    constexpr void SetFlag(FlagEnum Flag)   { mValue |= uint32(Flag); }
    constexpr void SetFlag(TFlags Flags)    { mValue |= Flags; }
    constexpr void ClearFlag(FlagEnum Flag) { mValue &= ~(uint32(Flag)); }
    constexpr void ClearFlag(TFlags Flags)  { mValue &= ~Flags; }

    constexpr void AssignFlag(FlagEnum Flag, bool Value)
    {
        if (Value)
            SetFlag(Flag);
        else
            ClearFlag(Flag);
    }

    [[nodiscard]] constexpr uint32 ToInt32() const { return mValue; }
    void Serialize(IArchive& rArc) { rArc.SerializePrimitive(mValue, SH_HexDisplay); }
};
// Note: When declaring flags for an enum class, use DECLARE_FLAGS_ENUMCLASS instead, defined below.
#define DECLARE_FLAGS(Enum, FlagTypeName) using FlagTypeName = TFlags<Enum>;

// Operator definitions for enum class flags
#define ENUMCLASS_OPERATOR_OR(LeftType, RightType) \
    constexpr int operator|(const LeftType Left, const RightType Right) \
    { \
        return (int) Left | (int) Right; \
    }

#define ENUMCLASS_OPERATOR_AND(LeftType, RightType) \
    constexpr int operator&(const LeftType Left, const RightType Right) \
    { \
        return (int) Left & (int) Right; \
    }

#define ENUMCLASS_OPERATOR_BITSHIFT_LEFT(LeftType, RightType) \
    constexpr int operator<<(const LeftType Left, const RightType Right) \
    { \
        return (int) Left << (int) Right; \
    }

#define ENUMCLASS_OPERATOR_BITSHIFT_RIGHT(LeftType, RightType) \
    constexpr int operator>>(const LeftType Left, const RightType Right) \
    { \
        return (int) Left >> (int) Right; \
    }

#define ENUMCLASS_OPERATOR_NOT(InType) \
    constexpr int operator~(const InType Value) \
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

