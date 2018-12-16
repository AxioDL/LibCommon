#ifndef CFILEINSTREAM_H
#define CFILEINSTREAM_H

#include "IInputStream.h"
#include "IOUtil.h"

class CFileInStream : public IInputStream
{
private:
    FILE *mpFStream;
    TString mName;
    uint32 mFileSize;

public:
    CFileInStream();
    CFileInStream(const TString& rkFile);
    CFileInStream(const TString& rkFile, EEndian FileEndianness);
    CFileInStream(const CFileInStream& rkSrc);
    ~CFileInStream();
    void Open(const TString& rkFile, EEndian FileEndianness);
    void Close();

    void ReadBytes(void *pDst, uint32 Count);
    bool Seek(int32 Offset, uint32 Origin);
    bool Seek64(int64 Offset, uint32 Origin);
    uint32 Tell() const;
    uint64 Tell64() const;
    bool EoF() const;
    bool IsValid() const;
    uint32 Size() const;
    TString FileName() const;
};

#endif // CFILEINSTREAM_H
