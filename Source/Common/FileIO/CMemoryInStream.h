#ifndef CMEMORYINSTREAM_H
#define CMEMORYINSTREAM_H

#include "IInputStream.h"

class CMemoryInStream : public IInputStream
{
    const char *mpDataStart;
    uint32 mDataSize;
    uint32 mPos;

public:
    CMemoryInStream();
    CMemoryInStream(const void *pkData, uint32 Size, IOUtil::EEndianness dataEndianness);
    ~CMemoryInStream();
    void SetData(const void *pkData, uint32 Size, IOUtil::EEndianness dataEndianness);

    void ReadBytes(void *pDst, uint32 Count);
    bool Seek(int32 Offset, uint32 Origin);
    uint32 Tell() const;
    bool EoF() const;
    bool IsValid() const;
    uint32 Size() const;
    void SetSize(uint32 Size);
    const void* Data() const;
    const void* DataAtPosition() const;
};

#endif // CMEMORYINSTREAM_H
