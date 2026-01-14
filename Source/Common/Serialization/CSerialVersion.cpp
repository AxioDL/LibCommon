#include "CSerialVersion.h"

#include "Common/CFourCC.h"
#include "Common/FileIO/IInputStream.h"
#include "Common/FileIO/IOutputStream.h"

CSerialVersion::CSerialVersion(IInputStream& rInput)
{
    Read(rInput);
}

void CSerialVersion::Read(IInputStream& rInput)
{
    mArchiveVersion = rInput.ReadU16();
    mFileVersion = rInput.ReadU16();
    CFourCC GameID(rInput);
    mGame = GameFrom4CC(GameID);
}

void CSerialVersion::Write(IOutputStream& rOutput) const
{
    rOutput.WriteU16(mArchiveVersion);
    rOutput.WriteU16(mFileVersion);
    CFourCC GameID = GameTo4CC(mGame);
    GameID.Write(rOutput);
}
