#include "EGame.h"
#include "CFourCC.h"
#include "Common/Serialization/IArchive.h"
#include <algorithm>
#include <array>

TString GetGameName(EGame Game)
{
    static constexpr std::array skGameNames{
        "Metroid Prime Demo",
        "Metroid Prime",
        "Metroid Prime 2: Echoes Demo",
        "Metroid Prime 2: Echoes",
        "Metroid Prime 3: Corruption E3 2006 Prototype",
        "Metroid Prime 3: Corruption",
        "Donkey Kong Country Returns",
        "Metroid Prime Remastered",
    };

    if (Game < EGame::PrimeDemo || Game > EGame::PrimeRemastered) {
        return "Unknown Game";
    }

    return skGameNames[static_cast<size_t>(Game)];
}

TString GetGameShortName(EGame Game)
{
    static constexpr std::array skGameNames{
        "MP1Demo",
        "MP1",
        "MP2Demo",
        "MP2",
        "MP3Proto",
        "MP3",
        "DKCR",
        "MP1R",
    };

    if (Game < EGame::PrimeDemo || Game > EGame::PrimeRemastered) {
        return "Unknown";
    }

    return skGameNames[static_cast<size_t>(Game)];
}

CFourCC GameTo4CC(EGame Game)
{
    static constexpr std::array skGame4CCs{
        CFourCC("MP1D"),
        CFourCC("MPRM"),
        CFourCC("MP2D"),
        CFourCC("MP2E"),
        CFourCC("MP3P"),
        CFourCC("MP3C"),
        CFourCC("DKCR"),
        CFourCC("MP1R"),
    };

    if (Game < EGame::PrimeDemo || Game > EGame::PrimeRemastered) {
        return CFourCC("UNKN");
    }

    return skGame4CCs[static_cast<size_t>(Game)];
}

EGame GameFrom4CC(CFourCC GameId)
{
    static constexpr std::array<std::pair<CFourCC, EGame>, 8> skIdToGame{{
        {CFourCC("MP1D"), EGame::PrimeDemo},
        {CFourCC("MPRM"), EGame::Prime},
        {CFourCC("MP2D"), EGame::EchoesDemo},
        {CFourCC("MP2E"), EGame::Echoes},
        {CFourCC("MP3P"), EGame::CorruptionProto},
        {CFourCC("MP3C"), EGame::Corruption},
        {CFourCC("DKCR"), EGame::DKCReturns},
        {CFourCC("MP1R"), EGame::PrimeRemastered},
    }};

    const auto iter = std::ranges::find_if(skIdToGame,
                                           [GameId](const auto& entry) { return entry.first == GameId; });
    if (iter == skIdToGame.cend()) {
        return EGame::Invalid;
    }

    return iter->second;
}

void Serialize(IArchive& rArc, EGame& rGame)
{
    // We serialize EGame as a fourCC in binary formats as a future-proofing measure.
    // Additionally, older versions of IArchive always serialized EGame as a fourCC.
    if (rArc.ArchiveVersion() < IArchive::eArVer_GameEnumClass || rArc.IsBinaryFormat())
    {
        CFourCC GameId;

        if (rArc.IsWriter())
        {
            GameId = GameTo4CC(rGame);
        }

        rArc.SerializePrimitive(GameId, 0);

        if (rArc.IsReader())
        {
            rGame = GameFrom4CC(GameId);
        }
    }
    else
    {
        DefaultEnumSerialize<EGame>(rArc, rGame);
    }
}

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

template <>
const CEnumNameMap TEnumReflection<ERegion>::skNameMap = {
    {0, "NTSC"},
    {1, "PAL"},
    {2, "JPN"},
    {-1, "Unknown"},
};
template <>
const int TEnumReflection<ERegion>::skErrorValue = -1;