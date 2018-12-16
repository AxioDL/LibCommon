#ifndef CFILEOUTSTREAM_H
#define CFILEOUTSTREAM_H

#include "IOutputStream.h"

class CFileOutStream : public IOutputStream
{
private:
    FILE *mpFStream;
    TString mName;
    uint32 mSize;

public:
    CFileOutStream();
    CFileOutStream(const TString& rkFile);
    CFileOutStream(const TString& rkFile, EEndian FileEndianness);
    CFileOutStream(const CFileOutStream& rkSrc);
    ~CFileOutStream();
    void Open(const TString& rkFile, EEndian);
    void Update(const TString& rkFile, EEndian FileEndianness);
    void Close();

    void WriteBytes(const void *pkSrc, uint32 Count);
    bool Seek(int32 Offset, uint32 Origin);
    bool Seek64(int64 Offset, uint32 Origin);
    uint32 Tell() const;
    uint64 Tell64() const;
    bool EoF() const;
    bool IsValid() const;
    uint32 Size() const;
    TString FileName() const;
};

#endif // CFILEOUTSTREAM_H
