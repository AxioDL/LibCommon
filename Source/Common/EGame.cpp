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
    };

    if (Game < EGame::PrimeDemo || Game > EGame::DKCReturns) {
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
    };

    if (Game < EGame::PrimeDemo || Game > EGame::DKCReturns) {
        return "Unknown";
    }

    return skGameNames[static_cast<size_t>(Game)];
}

CFourCC GameTo4CC(EGame Game)
{
    static constexpr std::array skGame4CCs{
        CFourCC(FOURCC('MP1D')),
        CFourCC(FOURCC('MPRM')),
        CFourCC(FOURCC('MP2D')),
        CFourCC(FOURCC('MP2E')),
        CFourCC(FOURCC('MP3P')),
        CFourCC(FOURCC('MP3C')),
        CFourCC(FOURCC('DKCR')),
    };

    if (Game < EGame::PrimeDemo || Game > EGame::DKCReturns) {
        return FOURCC('UNKN');
    }

    return skGame4CCs[static_cast<size_t>(Game)];
}

EGame GameFrom4CC(CFourCC GameId)
{
    static constexpr std::array<std::pair<uint32_t, EGame>, 7> skIdToGame{{
        {FOURCC('MP1D'), EGame::PrimeDemo},
        {FOURCC('MPRM'), EGame::Prime},
        {FOURCC('MP2D'), EGame::EchoesDemo},
        {FOURCC('MP2E'), EGame::Echoes},
        {FOURCC('MP3P'), EGame::CorruptionProto},
        {FOURCC('MP3C'), EGame::Corruption},
        {FOURCC('DKCR'), EGame::DKCReturns},
    }};

    const auto iter = std::find_if(skIdToGame.cbegin(), skIdToGame.cend(),
                                   [GameId](const auto& entry) { return entry.first == GameId.ToLong(); });
    if (iter == skIdToGame.cend())
    {
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
