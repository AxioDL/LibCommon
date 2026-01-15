#ifndef EGAME_H
#define EGAME_H

#include "TString.h"
#include "EnumReflection.h"

class CFourCC;
class IArchive;

//@todo I'm not really happy with EGame being in Common but atm can't think of
// any other decent way to integrate it with IArchive unfortunately
enum class EGame
{
    PrimeDemo,
    Prime,
    EchoesDemo,
    Echoes,
    CorruptionProto,
    Corruption,
    DKCReturns,
    PrimeRemastered,

    Max,
    Invalid = -1
};
template <>
const CEnumNameMap TEnumReflection<EGame>::skNameMap = {
    {0, "PrimeDemo"},
    {1, "Prime"},
    {2, "EchoesDemo"},
    {3, "Echoes"},
    {4, "CorruptionProto"},
    {5, "Corruption"},
    {6, "DKCReturns"},
    {7, "PrimeRemastered"},
    {8, "Max"},
    {-1, "Invalid"},
};
template <>
const int TEnumReflection<EGame>::skErrorValue = -1;

TString GetGameName(EGame Game);
TString GetGameShortName(EGame Game);
CFourCC GameTo4CC(EGame Game);
EGame GameFrom4CC(CFourCC GameId);
void Serialize(IArchive& rArc, EGame& rGame);

// ERegion
enum class ERegion
{
    NTSC,
    PAL,
    JPN,
    Unknown = -1
};
template <>
const CEnumNameMap TEnumReflection<ERegion>::skNameMap = {
    {0, "NTSC"},
    {1, "PAL"},
    {2, "JPN"},
    {-1, "Unknown"},
};
template <>
const int TEnumReflection<ERegion>::skErrorValue = -1;

#endif // EGAME_H
