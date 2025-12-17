#ifndef CSERIALVERSION
#define CSERIALVERSION

#include "Common/BasicTypes.h"
#include "Common/EGame.h"

class IInputStream;
class IOutputStream;

class CSerialVersion
{
    uint16 mArchiveVersion = 0;
    uint16 mFileVersion = 0;
    EGame mGame{};

public:
    CSerialVersion();
    explicit CSerialVersion(uint16 ArchiveVer, uint16 FileVer, EGame Game);
    explicit CSerialVersion(IInputStream& rInput);

    void Read(IInputStream& rInput);
    void Write(IOutputStream& rOutput);

    uint16 ArchiveVersion() const    { return mArchiveVersion; }
    uint16 FileVersion() const       { return mFileVersion; }
    EGame Game() const               { return mGame; }
};

#endif // CSERIALVERSION

