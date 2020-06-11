#include "CAssetID.h"
#include "CRandom.h"
#include "TString.h"

void CAssetID::Write(IOutputStream& rOutput, EIDLength ForcedLength /*= eInvalidIDLength*/) const
{
    const auto Length = (ForcedLength == EIDLength::kInvalidIDLength ? mLength : ForcedLength);

    if (Length == EIDLength::k32Bit)
        rOutput.WriteLong(ToLong());
    else
        rOutput.WriteLongLong(ToLongLong());
}

CAssetID::CAssetID(IInputStream& rInput, EIDLength Length)
    : mLength(Length)
{
    if (Length == EIDLength::k32Bit)
        mID = static_cast<uint64>(rInput.ReadLong()) & 0xFFFFFFFF;
    else
        mID = rInput.ReadLongLong();
}

CAssetID::CAssetID(IInputStream& rInput, EGame Game)
{
    *this = CAssetID(rInput, (Game <= EGame::Echoes ? EIDLength::k32Bit : EIDLength::k64Bit));
}

TString CAssetID::ToString(EIDLength ForcedLength /*= eInvalidIDLength*/) const
{
    const auto Length = (ForcedLength == EIDLength::kInvalidIDLength ? mLength : ForcedLength);

    if (Length == EIDLength::k32Bit)
        return TString::HexString(ToLong(), 8, false, true);
    else
        return TString::FromInt64(ToLongLong(), 16, 16).ToUpper();
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
    if (Length != EIDLength::k64Bit)
    {
        CAssetID Out;
        Out.mLength = EIDLength::k32Bit;
        Out.mID = static_cast<uint64>(CRandom::GlobalRandom()->Int32()) & 0xFFFFFFFF;
        return Out;
    }
    else
    {
        CAssetID Out;
        Out.mLength = EIDLength::k64Bit;
        Out.mID = static_cast<uint64>(CRandom::GlobalRandom()->Int64());
        return Out;
    }
}

CAssetID CAssetID::RandomID(EGame Game)
{
    return RandomID(GameIDLength(Game));
}

// ************ STATIC MEMBER INITIALIZATION ************
CAssetID CAssetID::skInvalidID32 = CAssetID(UINT64_MAX, EIDLength::k32Bit);
CAssetID CAssetID::skInvalidID64 = CAssetID(UINT64_MAX, EIDLength::k64Bit);
