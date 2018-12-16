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
    EIDLength mLength;
    uint64 mID;

public:
    CAssetID();
    CAssetID(uint64 ID);
    CAssetID(uint64 ID, EIDLength Length);
    CAssetID(IInputStream& rInput, EIDLength Length);
    CAssetID(IInputStream& rInput, EGame Game);
    void Write(IOutputStream& rOutput, EIDLength ForcedLength = kInvalidIDLength) const;
    TString ToString(EIDLength ForcedLength = kInvalidIDLength) const;
    bool IsValid() const;

    // Operators
    inline void operator= (const uint64& rkInput)           { *this = CAssetID(rkInput); }
    inline bool operator==(const CAssetID& rkOther) const   { return mLength == rkOther.mLength && mID == rkOther.mID; }
    inline bool operator!=(const CAssetID& rkOther) const   { return mLength != rkOther.mLength || mID != rkOther.mID; }
    inline bool operator> (const CAssetID& rkOther) const   { return mLength >= rkOther.mLength && mID > rkOther.mID; }
    inline bool operator>=(const CAssetID& rkOther) const   { return mLength >= rkOther.mLength && mID >= rkOther.mID; }
    inline bool operator< (const CAssetID& rkOther) const   { return mLength <  rkOther.mLength || mID < rkOther.mID; }
    inline bool operator<=(const CAssetID& rkOther) const   { return mLength <  rkOther.mLength || mID <= rkOther.mID; }
    inline bool operator==(uint64 Other) const              { return mID == Other; }
    inline bool operator!=(uint64 Other) const              { return mID != Other; }

    // Accessors
    inline uint32 ToLong() const            { return (uint32) mID; }
    inline uint64 ToLongLong() const        { return mID; }
    inline EIDLength Length() const         { return mLength; }
    inline void SetLength(EIDLength Length) { mLength = Length; }

    // Static
    static CAssetID FromString(const TString& rkString);
    static CAssetID RandomID();

    inline static EIDLength GameIDLength(EGame Game)        { return (Game == EGame::Invalid ? kInvalidIDLength : (Game <= EGame::Echoes ? k32Bit : k64Bit)); }
    inline static CAssetID InvalidID(EIDLength IDLength)    { return (IDLength == k32Bit ? skInvalidID32 : skInvalidID64); }
    inline static CAssetID InvalidID(EGame Game)            { return InvalidID(Game <= EGame::Echoes ? k32Bit : k64Bit); }

    static CAssetID skInvalidID32;
    static CAssetID skInvalidID64;
};

#endif // CASSETID_H
