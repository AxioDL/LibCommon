#ifndef AXIO_CASSETID_H
#define AXIO_CASSETID_H

#include "EGame.h"
#include "TString.h"

#include <cstdint>

class IInputStream;
class IOutputStream;

enum class EIDLength
{
    k32Bit = 4,
    k64Bit = 8,
    kInvalidIDLength = 0
};

class CAssetID
{
    EIDLength mLength = EIDLength::kInvalidIDLength;
    uint64_t mID = UINT64_MAX;

public:
    constexpr CAssetID() = default;
    constexpr CAssetID(uint64_t ID) : mID{ID} {
        // This constructor is intended to be used with both 32-bit and 64-bit input values
        // 64-bit - check for valid content in upper 32 bits (at least one bit set + one bit unset)
        if ((ID & 0xFFFFFFFF00000000) && (~ID & 0xFFFFFFFF00000000)) {
            mLength = EIDLength::k64Bit;
        } else {
            mLength = EIDLength::k32Bit;
            mID &= 0xFFFFFFFF;
        }
    }
    constexpr CAssetID(uint64_t ID, EIDLength Length) : mLength{Length}, mID{ID} {
        if (Length == EIDLength::k32Bit) {
            mID &= 0xFFFFFFFF;
        }
    }
    explicit CAssetID(IInputStream& rInput, EIDLength Length);
    explicit CAssetID(IInputStream& rInput, EGame Game);

    void Write(IOutputStream& rOutput, EIDLength ForcedLength = EIDLength::kInvalidIDLength) const;
    TString ToString(EIDLength ForcedLength = EIDLength::kInvalidIDLength) const;

    [[nodiscard]] constexpr bool IsValid() const {
        return (mID != 0 && mLength != EIDLength::kInvalidIDLength && mID != InvalidID(mLength).mID);
    }

    // Operators
    constexpr CAssetID& operator=(const uint64_t& rkInput)
    {
        *this = CAssetID(rkInput);
        return *this;
    }
    [[nodiscard]] constexpr bool operator==(const CAssetID& rkOther) const   { return mLength == rkOther.mLength && mID == rkOther.mID; }
    [[nodiscard]] constexpr bool operator!=(const CAssetID& rkOther) const   { return mLength != rkOther.mLength || mID != rkOther.mID; }
    [[nodiscard]] constexpr bool operator> (const CAssetID& rkOther) const   { return mLength >= rkOther.mLength && mID > rkOther.mID; }
    [[nodiscard]] constexpr bool operator>=(const CAssetID& rkOther) const   { return mLength >= rkOther.mLength && mID >= rkOther.mID; }
    [[nodiscard]] constexpr bool operator< (const CAssetID& rkOther) const   { return mLength <  rkOther.mLength || mID < rkOther.mID; }
    [[nodiscard]] constexpr bool operator<=(const CAssetID& rkOther) const   { return mLength <  rkOther.mLength || mID <= rkOther.mID; }
    [[nodiscard]] constexpr bool operator==(uint64_t Other) const            { return mID == Other; }
    [[nodiscard]] constexpr bool operator!=(uint64_t Other) const            { return mID != Other; }

    // Accessors
    [[nodiscard]] constexpr uint32_t ToU32() const   { return uint32_t(mID); }
    [[nodiscard]] constexpr uint64_t ToU64() const   { return mID; }
    [[nodiscard]] constexpr EIDLength Length() const { return mLength; }
    constexpr void SetLength(EIDLength Length)       { mLength = Length; }

    // Static
    [[nodiscard]] static CAssetID FromString(const TString& rkString);
    [[nodiscard]] static CAssetID RandomID(EIDLength Length);
    [[nodiscard]] static CAssetID RandomID(EGame Game);

    [[nodiscard]] static constexpr EIDLength GameIDLength(EGame Game)
    {
        return Game == EGame::Invalid ? EIDLength::kInvalidIDLength : (Game <= EGame::Echoes ? EIDLength::k32Bit : EIDLength::k64Bit);
    }

    [[nodiscard]] static constexpr CAssetID InvalidID(EIDLength IDLength)
    {
        return IDLength == EIDLength::k32Bit ? skInvalidID32 : skInvalidID64;
    }

    [[nodiscard]] static constexpr CAssetID InvalidID(EGame Game)
    {
        return InvalidID(Game <= EGame::Echoes ? EIDLength::k32Bit : EIDLength::k64Bit);
    }

    static const CAssetID skInvalidID32;
    static const CAssetID skInvalidID64;
};

#endif // AXIO_CASSETID_H
