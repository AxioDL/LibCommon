#ifndef FLAGS_H
#define FLAGS_H

#include "BasicTypes.h"
#include "Serialization/IArchive.h"

#include <compare>
#include <type_traits>

template<typename FlagEnum>
requires(std::is_enum_v<FlagEnum>)
class TFlags
{
    using UnsignedT = std::make_unsigned_t<std::underlying_type_t<FlagEnum>>;

    UnsignedT mValue = 0;

public:
    constexpr TFlags() = default;
    constexpr TFlags(FlagEnum Val) : mValue(UnsignedT(Val)) {}
    constexpr explicit TFlags(UnsignedT Val) : mValue(Val) {}

    [[nodiscard]] constexpr explicit operator bool() const { return mValue != 0; }
    [[nodiscard]] constexpr auto operator<=>(const TFlags&) const noexcept = default;

    [[nodiscard]] constexpr TFlags operator~() const { return TFlags(UnsignedT(~mValue)); }

#define FLAG_BITWISE_OP_ENUM(op)                           \
    constexpr TFlags operator op (FlagEnum val) const {    \
        return TFlags(FlagEnum(mValue op UnsignedT(val))); \
    }                                                      \
    constexpr TFlags operator op (TFlags val) const {      \
        return TFlags(mValue op val.mValue);               \
    }
    FLAG_BITWISE_OP_ENUM(|)
    FLAG_BITWISE_OP_ENUM(&)
    FLAG_BITWISE_OP_ENUM(^)
#undef FLAG_BITWISE_OP_ENUM

#define FLAG_BITWISE_OP_ASSIGN_ENUM(op)            \
    constexpr TFlags& operator op (FlagEnum val) { \
        mValue op UnsignedT(val);                  \
        return *this;                              \
    }                                              \
    constexpr TFlags& operator op (TFlags val) {   \
        mValue op val.mValue;                      \
        return *this;                              \
    }
    FLAG_BITWISE_OP_ASSIGN_ENUM(|=)
    FLAG_BITWISE_OP_ASSIGN_ENUM(&=)
    FLAG_BITWISE_OP_ASSIGN_ENUM(^=)
#undef FLAG_BITWISE_OP_ASSIGN_ENUM

    [[nodiscard]] constexpr bool HasFlag(FlagEnum Flag) const    { return ((mValue & UnsignedT(Flag)) != 0); }
    [[nodiscard]] constexpr bool HasAnyFlags(TFlags Flags) const { return ((mValue & Flags.mValue) != 0); }
    [[nodiscard]] constexpr bool HasAllFlags(TFlags Flags) const { return ((mValue & Flags.mValue) == Flags.mValue); }
    constexpr void SetFlag(FlagEnum Flag)   { mValue |= UnsignedT(Flag); }
    constexpr void SetFlag(TFlags Flags)    { mValue |= Flags.mValue; }
    constexpr void ClearFlag(FlagEnum Flag) { mValue &= ~(UnsignedT(Flag)); }
    constexpr void ClearFlag(TFlags Flags)  { mValue &= ~Flags.mValue; }

    constexpr void AssignFlag(FlagEnum Flag, bool Value)
    {
        if (Value)
            SetFlag(Flag);
        else
            ClearFlag(Flag);
    }

    constexpr void Reset(FlagEnum value) { mValue = UnsignedT(value); }
    constexpr void Reset(TFlags flags) { mValue = flags.mValue; }

    [[nodiscard]] constexpr UnsignedT Value() const { return mValue; }
    void Serialize(IArchive& rArc) { rArc.SerializePrimitive(mValue, SH_HexDisplay); }
};
// Note: When declaring flags for an enum class, use DECLARE_FLAGS_ENUMCLASS instead, defined below.
#define AXIO_DECLARE_FLAGS(Enum, FlagTypeName) using FlagTypeName = TFlags<Enum>;

#define AXIO_DEFINE_ENUM_OPS(EnumT)                                             \
    constexpr EnumT operator|(EnumT lhs, EnumT rhs)                             \
    {                                                                           \
        using Underlying = std::make_unsigned_t<std::underlying_type_t<EnumT>>; \
        return EnumT(Underlying(lhs) | Underlying(rhs));                        \
    }                                                                           \
    constexpr EnumT operator&(EnumT lhs, EnumT rhs)                             \
    {                                                                           \
        using Underlying = std::make_unsigned_t<std::underlying_type_t<EnumT>>; \
        return EnumT(Underlying(lhs) & Underlying(rhs));                        \
    }                                                                           \
    constexpr EnumT operator^(EnumT lhs, EnumT rhs)                             \
    {                                                                           \
        using Underlying = std::make_unsigned_t<std::underlying_type_t<EnumT>>; \
        return EnumT(Underlying(lhs) | Underlying(rhs));                        \
    }                                                                           \
    constexpr EnumT operator~(EnumT value)                                      \
    {                                                                           \
        using Underlying = std::make_unsigned_t<std::underlying_type_t<EnumT>>; \
        return EnumT(~Underlying(value));                                       \
    }                                                                           \
    constexpr EnumT& operator|=(EnumT& lhs, EnumT rhs)                          \
    {                                                                           \
        lhs = lhs | rhs;                                                        \
        return lhs;                                                             \
    }                                                                           \
    constexpr EnumT& operator&=(EnumT& lhs, EnumT rhs)                          \
    {                                                                           \
        lhs = lhs & rhs;                                                        \
        return lhs;                                                             \
    }                                                                           \
    constexpr EnumT& operator^=(EnumT& lhs, EnumT rhs)                          \
    {                                                                           \
        lhs = lhs ^ rhs;                                                        \
        return lhs;                                                             \
    }                                                                           \
    template <std::integral Shift>                                              \
    constexpr EnumT operator<<(EnumT lhs, Shift value)                          \
    {                                                                           \
        using Underlying = std::make_unsigned_t<std::underlying_type_t<EnumT>>; \
        return EnumT(Underlying(lhs) << value);                                 \
    }                                                                           \
    template <std::integral Shift>                                              \
    constexpr EnumT operator>>(EnumT lhs, Shift value)                          \
    {                                                                           \
        using Underlying = std::make_unsigned_t<std::underlying_type_t<EnumT>>; \
        return EnumT(Underlying(lhs) >> value);                                 \
    }                                                                           \
    template <std::integral Shift>                                              \
    constexpr EnumT& operator<<=(EnumT& lhs, Shift value)                       \
    {                                                                           \
        lhs = lhs << value;                                                     \
        return lhs;                                                             \
    }                                                                           \
    template <std::integral Shift>                                              \
    constexpr EnumT& operator>>=(EnumT& lhs, Shift value)                       \
    {                                                                           \
        lhs = lhs >> value;                                                     \
        return lhs;                                                             \
    }

#define AXIO_DECLARE_FLAGS_ENUMCLASS(Enum, FlagTypeName) \
    AXIO_DECLARE_FLAGS(Enum, FlagTypeName)               \
    AXIO_DEFINE_ENUM_OPS(Enum)

#endif // FLAGS_H

