#ifndef CSERIALVERSION
#define CSERIALVERSION

#include "Common/BasicTypes.h"
#include "Common/EGame.h"
#include "Common/FileIO.h"

class CSerialVersion
{
    u16 mArchiveVersion;
    u16 mFileVersion;
    EGame mGame;

public:
    CSerialVersion();
    CSerialVersion(u16 ArchiveVer, u16 FileVer, EGame Game);
    CSerialVersion(IInputStream& rInput);
    void Read(IInputStream& rInput);
    void Write(IOutputStream& rOutput);

    inline u16 ArchiveVersion() const   { return mArchiveVersion; }
    inline u16 FileVersion() const      { return mFileVersion; }
    inline EGame Game() const           { return mGame; }
};

#endif // CSERIALVERSION

