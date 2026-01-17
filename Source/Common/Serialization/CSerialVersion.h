#ifndef AXIO_CSERIALVERSION_H
#define AXIO_CSERIALVERSION_H

#include "Common/BasicTypes.h"
#include "Common/EGame.h"

class IInputStream;
class IOutputStream;

class CSerialVersion
{
    uint16_t mArchiveVersion = 0;
    uint16_t mFileVersion = 0;
    EGame mGame{};

public:
    constexpr CSerialVersion() = default;
    explicit constexpr CSerialVersion(uint16_t ArchiveVer, uint16_t FileVer, EGame Game)
        : mArchiveVersion{ArchiveVer}, mFileVersion{FileVer}, mGame{Game} {}

    explicit CSerialVersion(IInputStream& rInput);

    void Read(IInputStream& rInput);
    void Write(IOutputStream& rOutput) const;

    uint16_t ArchiveVersion() const  { return mArchiveVersion; }
    uint16_t FileVersion() const     { return mFileVersion; }
    EGame Game() const               { return mGame; }
};

#endif // AXIO_CSERIALVERSION_H

