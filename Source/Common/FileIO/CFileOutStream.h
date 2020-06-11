#ifndef CFILEOUTSTREAM_H
#define CFILEOUTSTREAM_H

#include "IOutputStream.h"

class CFileOutStream : public IOutputStream
{
private:
    FILE *mpFStream = nullptr;
    TString mName;
    uint32 mSize = 0;

public:
    CFileOutStream();
    CFileOutStream(const TString& rkFile);
    CFileOutStream(const TString& rkFile, EEndian FileEndianness);
    CFileOutStream(const CFileOutStream& rkSrc);
    ~CFileOutStream() override;
    void Open(const TString& rkFile, EEndian);
    void Update(const TString& rkFile, EEndian FileEndianness);
    void Close();

    void WriteBytes(const void* pkSrc, uint32 Count) override;
    bool Seek(int32 Offset, uint32 Origin) override;
    bool Seek64(int64 Offset, uint32 Origin) override;
    uint32 Tell() const override;
    uint64 Tell64() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32 Size() const override;
    TString FileName() const;
};

#endif // CFILEOUTSTREAM_H
