#include "CAssetID.h"
#include "CRandom.h"
#include "TString.h"

CAssetID::CAssetID()
    : mLength(kInvalidIDLength)
    , mID(0xFFFFFFFFFFFFFFFF)
{
}

CAssetID::CAssetID(uint64 ID)
    : mID(ID)
{
    // This constructor is intended to be used with both 32-bit and 64-bit input values
    // 64-bit - check for valid content in upper 32 bits (at least one bit set + one bit unset)
    if ((ID & 0xFFFFFFFF00000000) && (~ID & 0xFFFFFFFF00000000))
        mLength = k64Bit;

    // 32-bit
    else
    {
        mLength = k32Bit;
        mID &= 0xFFFFFFFF;
    }
}

CAssetID::CAssetID(uint64 ID, EIDLength Length)
    : mID(ID)
    , mLength(Length)
{
    if (Length == k32Bit)
        mID &= 0xFFFFFFFF;
}

void CAssetID::Write(IOutputStream& rOutput, EIDLength ForcedLength /*= eInvalidIDLength*/) const
{
    EIDLength Length = (ForcedLength == kInvalidIDLength ? mLength : ForcedLength);

    if (Length == k32Bit)
        rOutput.WriteLong(ToLong());
    else
        rOutput.WriteLongLong(ToLongLong());
}

CAssetID::CAssetID(IInputStream& rInput, EIDLength Length)
    : mLength(Length)
{
    if (Length == k32Bit)   mID = ((uint64) rInput.ReadLong()) & 0xFFFFFFFF;
    else                    mID = rInput.ReadLongLong();
}

CAssetID::CAssetID(IInputStream& rInput, EGame Game)
{
    *this = CAssetID(rInput, (Game <= EGame::Echoes ? k32Bit : k64Bit));
}

TString CAssetID::ToString(EIDLength ForcedLength /*= eInvalidIDLength*/) const
{
    EIDLength Length = (ForcedLength == kInvalidIDLength ? mLength : ForcedLength);

    if (Length == k32Bit)
        return TString::HexString(ToLong(), 8, false, true);
    else
        return TString::FromInt64(ToLongLong(), 16, 16).ToUpper();
}

bool CAssetID::IsValid() const
{
    return (mID != 0 && mLength != kInvalidIDLength && mID != InvalidID(mLength).mID);
}

// ************ STATIC ************
CAssetID CAssetID::FromString(const TString& rkString)
{
    // If the input is a hex ID in string form, then preserve it... otherwise, generate an ID by hashing the string
    TString Name = rkString.GetFileName(false);
    uint32 NameLength = Name.Length();

    if (Name.IsHexString())
    {
        if (NameLength == 8)  return CAssetID(Name.ToInt32(16));
        if (NameLength == 16) return CAssetID(Name.ToInt64(16));
    }

    return CAssetID(rkString.Hash64());
}

CAssetID CAssetID::RandomID(EIDLength Length)
{
    if (Length != k64Bit)
    {
        CAssetID Out;
        Out.mLength = k32Bit;
        Out.mID = ((uint64) CRandom::GlobalRandom()->Int32()) & 0xFFFFFFFF;
        return Out;
    }
    else
    {
        CAssetID Out;
        Out.mLength = k64Bit;
        Out.mID = (uint64) CRandom::GlobalRandom()->Int64();
        return Out;
    }
}

CAssetID CAssetID::RandomID(EGame Game)
{
    return RandomID( GameIDLength(Game) );
}

// ************ STATIC MEMBER INITIALIZATION ************
CAssetID CAssetID::skInvalidID32 = CAssetID((uint64) -1, k32Bit);
CAssetID CAssetID::skInvalidID64 = CAssetID((uint64) -1, k64Bit);
