#ifndef CASSETID_H
#define CASSETID_H

#include "BasicTypes.h"
#include "EGame.h"
#include "FileIO.h"
#include "TString.h"

enum EIDLength
{
    k32Bit = 4,
    k64Bit = 8,
    kInvalidIDLength = 0
};

class CAssetID
{
    EIDLength mLength = kInvalidIDLength;
    uint64 mID = UINT64_MAX;

public:
    constexpr CAssetID() = default;
    constexpr CAssetID(uint64 ID) : mID{ID} {
        // This constructor is intended to be used with both 32-bit and 64-bit input values
        // 64-bit - check for valid content in upper 32 bits (at least one bit set + one bit unset)
        if ((ID & 0xFFFFFFFF00000000) && (~ID & 0xFFFFFFFF00000000)) {
            mLength = k64Bit;
        } else {
            mLength = k32Bit;
            mID &= 0xFFFFFFFF;
        }
    }
    constexpr CAssetID(uint64 ID, EIDLength Length) : mLength{Length}, mID{ID} {
        if (Length == k32Bit) {
            mID &= 0xFFFFFFFF;
        }
    }
    explicit CAssetID(IInputStream& rInput, EIDLength Length);
    explicit CAssetID(IInputStream& rInput, EGame Game);

    void Write(IOutputStream& rOutput, EIDLength ForcedLength = kInvalidIDLength) const;
    TString ToString(EIDLength ForcedLength = kInvalidIDLength) const;

    [[nodiscard]] constexpr bool IsValid() const {
        return (mID != 0 && mLength != kInvalidIDLength && mID != InvalidID(mLength).mID);
    }

    // Operators
    constexpr void operator= (const uint64& rkInput)           { *this = CAssetID(rkInput); }
    [[nodiscard]] constexpr bool operator==(const CAssetID& rkOther) const   { return mLength == rkOther.mLength && mID == rkOther.mID; }
    [[nodiscard]] constexpr bool operator!=(const CAssetID& rkOther) const   { return mLength != rkOther.mLength || mID != rkOther.mID; }
    [[nodiscard]] constexpr bool operator> (const CAssetID& rkOther) const   { return mLength >= rkOther.mLength && mID > rkOther.mID; }
    [[nodiscard]] constexpr bool operator>=(const CAssetID& rkOther) const   { return mLength >= rkOther.mLength && mID >= rkOther.mID; }
    [[nodiscard]] constexpr bool operator< (const CAssetID& rkOther) const   { return mLength <  rkOther.mLength || mID < rkOther.mID; }
    [[nodiscard]] constexpr bool operator<=(const CAssetID& rkOther) const   { return mLength <  rkOther.mLength || mID <= rkOther.mID; }
    [[nodiscard]] constexpr bool operator==(uint64 Other) const              { return mID == Other; }
    [[nodiscard]] constexpr bool operator!=(uint64 Other) const              { return mID != Other; }

    // Accessors
    [[nodiscard]] constexpr uint32 ToLong() const            { return uint32(mID); }
    [[nodiscard]] constexpr uint64 ToLongLong() const        { return mID; }
    [[nodiscard]] constexpr EIDLength Length() const         { return mLength; }
    constexpr void SetLength(EIDLength Length) { mLength = Length; }

    // Static
    [[nodiscard]] static CAssetID FromString(const TString& rkString);
    [[nodiscard]] static CAssetID RandomID(EIDLength Length);
    [[nodiscard]] static CAssetID RandomID(EGame Game);

    [[nodiscard]] static constexpr EIDLength GameIDLength(EGame Game)     { return (Game == EGame::Invalid ? kInvalidIDLength : (Game <= EGame::Echoes ? k32Bit : k64Bit)); }
    [[nodiscard]] static constexpr CAssetID InvalidID(EIDLength IDLength) { return (IDLength == k32Bit ? skInvalidID32 : skInvalidID64); }
    [[nodiscard]] static constexpr CAssetID InvalidID(EGame Game)         { return InvalidID(Game <= EGame::Echoes ? k32Bit : k64Bit); }

    static CAssetID skInvalidID32;
    static CAssetID skInvalidID64;
};

#endif // CASSETID_H
