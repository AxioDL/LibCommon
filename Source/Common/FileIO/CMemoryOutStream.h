#ifndef CMEMORYOUTSTREAM_H
#define CMEMORYOUTSTREAM_H

#include "IOutputStream.h"

class CMemoryOutStream : public IOutputStream
{
    char *mpDataStart = nullptr;
    uint32_t mDataSize = 0;
    uint32_t mPos = 0;
    uint32_t mUsed = 0;

public:
    CMemoryOutStream();
    explicit CMemoryOutStream(void *pData, uint32_t Size, std::endian mDataEndianness);
    ~CMemoryOutStream() override;

    void SetData(void *pData, uint32_t Size, std::endian mDataEndianness);

    void WriteBytes(const void* pkSrc, uint32_t Count) override;
    bool Seek(int32_t Offset, uint32_t Origin) override;
    uint32_t Tell() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32_t Size() const override;
    uint32_t SpaceUsed() const;
    void SetSize(uint32_t Size);
    void* Data() const;
    void* DataAtPosition() const;
};

#endif // CMEMORYOUTSTREAM_H
