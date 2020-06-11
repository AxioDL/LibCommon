#ifndef CFILEINSTREAM_H
#define CFILEINSTREAM_H

#include "IInputStream.h"
#include "IOUtil.h"

class CFileInStream : public IInputStream
{
private:
    FILE *mpFStream = nullptr;
    TString mName;
    uint32 mFileSize = 0;

public:
    CFileInStream();
    CFileInStream(const TString& rkFile);
    CFileInStream(const TString& rkFile, EEndian FileEndianness);
    CFileInStream(const CFileInStream& rkSrc);
    ~CFileInStream() override;
    void Open(const TString& rkFile, EEndian FileEndianness);
    void Close();

    void ReadBytes(void* pDst, uint32 Count) override;
    bool Seek(int32 Offset, uint32 Origin) override;
    bool Seek64(int64 Offset, uint32 Origin) override;
    uint32 Tell() const override;
    uint64 Tell64() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32 Size() const override;
    TString FileName() const;
};

#endif // CFILEINSTREAM_H
