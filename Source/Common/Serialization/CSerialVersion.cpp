#include "CSerialVersion.h"

#include "Common/CFourCC.h"
#include "Common/FileIO/IInputStream.h"
#include "Common/FileIO/IOutputStream.h"

CSerialVersion::CSerialVersion() = default;

CSerialVersion::CSerialVersion(uint16 ArchiveVer, uint16 FileVer, EGame Game)
    : mArchiveVersion(ArchiveVer)
    , mFileVersion(FileVer)
    , mGame(Game)
{
}

CSerialVersion::CSerialVersion(IInputStream& rInput)
{
    Read(rInput);
}

void CSerialVersion::Read(IInputStream& rInput)
{
    mArchiveVersion = rInput.ReadUShort();
    mFileVersion = rInput.ReadUShort();
    CFourCC GameID(rInput);
    mGame = GameFrom4CC(GameID);
}

void CSerialVersion::Write(IOutputStream& rOutput)
{
    rOutput.WriteUShort(mArchiveVersion);
    rOutput.WriteUShort(mFileVersion);
    CFourCC GameID = GameTo4CC(mGame);
    GameID.Write(rOutput);
}
