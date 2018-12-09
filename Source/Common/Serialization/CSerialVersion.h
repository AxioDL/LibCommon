#ifndef CSERIALVERSION
#define CSERIALVERSION

#include "Common/BasicTypes.h"
#include "Common/EGame.h"
#include "Common/FileIO.h"

class CSerialVersion
{
    uint16 mArchiveVersion;
    uint16 mFileVersion;
    EGame mGame;

public:
    CSerialVersion();
    CSerialVersion(uint16 ArchiveVer, uint16 FileVer, EGame Game);
    CSerialVersion(IInputStream& rInput);
    void Read(IInputStream& rInput);
    void Write(IOutputStream& rOutput);

    inline uint16 ArchiveVersion() const    { return mArchiveVersion; }
    inline uint16 FileVersion() const       { return mFileVersion; }
    inline EGame Game() const               { return mGame; }
};

#endif // CSERIALVERSION

