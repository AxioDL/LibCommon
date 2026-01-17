#ifndef AXIO_CFILEINSTREAM_H
#define AXIO_CFILEINSTREAM_H

#include "IInputStream.h"

class CFileInStream : public IInputStream
{
private:
    FILE *mpFStream = nullptr;
    TString mName;
    uint32_t mFileSize = 0;

public:
    CFileInStream();
    explicit CFileInStream(const TString& rkFile);
    explicit CFileInStream(const TString& rkFile, std::endian FileEndianness);
    CFileInStream(const CFileInStream& rkSrc);
    ~CFileInStream() override;

    void Open(const TString& rkFile, std::endian FileEndianness);
    void Close();

    void ReadBytes(void* pDst, uint32_t Count) override;
    bool Seek(int32_t Offset, uint32_t Origin) override;
    bool Seek64(int64_t Offset, uint32_t Origin) override;
    uint32_t Tell() const override;
    uint64_t Tell64() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32_t Size() const override;
    TString FileName() const;
};

#endif // AXIO_CFILEINSTREAM_H
