#ifndef AXIO_CFILEOUTSTREAM_H
#define AXIO_CFILEOUTSTREAM_H

#include "IOutputStream.h"

class CFileOutStream : public IOutputStream
{
private:
    FILE *mpFStream = nullptr;
    TString mName;
    uint32_t mSize = 0;

public:
    CFileOutStream();
    explicit CFileOutStream(const TString& rkFile, std::endian FileEndianness);
    ~CFileOutStream() override;

    CFileOutStream(const CFileOutStream&) = delete;
    CFileOutStream& operator=(const CFileOutStream&) = delete;

    CFileOutStream(CFileOutStream&& Other) noexcept;
    CFileOutStream& operator=(CFileOutStream&& Other) noexcept;

    void Open(const TString& rkFile, std::endian);
    void Update(const TString& rkFile, std::endian FileEndianness);
    void Close();

    void WriteBytes(const void* pkSrc, uint32_t Count) override;
    bool Seek(int32_t Offset, uint32_t Origin) override;
    bool Seek64(int64_t Offset, uint32_t Origin) override;
    uint32_t Tell() const override;
    uint64_t Tell64() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32_t Size() const override;
    TString FileName() const;
};

#endif // AXIO_CFILEOUTSTREAM_H
