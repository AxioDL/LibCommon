#ifndef CMEMORYINSTREAM_H
#define CMEMORYINSTREAM_H

#include "IInputStream.h"

class CMemoryInStream : public IInputStream
{
    const char *mpDataStart = nullptr;
    uint32 mDataSize = 0;
    uint32 mPos = 0;

public:
    CMemoryInStream();
    CMemoryInStream(const void *pkData, uint32 Size, EEndian dataEndianness);
    ~CMemoryInStream() override;
    void SetData(const void *pkData, uint32 Size, EEndian dataEndianness);

    void ReadBytes(void* pDst, uint32 Count) override;
    bool Seek(int32 Offset, uint32 Origin) override;
    uint32 Tell() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32 Size() const override;
    void SetSize(uint32 Size);
    const void* Data() const;
    const void* DataAtPosition() const;
};

#endif // CMEMORYINSTREAM_H
