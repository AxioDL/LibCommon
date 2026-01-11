#ifndef CMEMORYINSTREAM_H
#define CMEMORYINSTREAM_H

#include "IInputStream.h"

class CMemoryInStream : public IInputStream
{
    const char *mpDataStart = nullptr;
    uint32_t mDataSize = 0;
    uint32_t mPos = 0;

public:
    CMemoryInStream();
    explicit CMemoryInStream(const void *pkData, uint32_t Size, std::endian dataEndianness);
    ~CMemoryInStream() override;

    void SetData(const void *pkData, uint32_t Size, std::endian dataEndianness);

    void ReadBytes(void* pDst, uint32_t Count) override;
    bool Seek(int32_t Offset, uint32_t Origin) override;
    uint32_t Tell() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32_t Size() const override;
    void SetSize(uint32_t Size);
    const void* Data() const;
    const void* DataAtPosition() const;
};

#endif // CMEMORYINSTREAM_H
