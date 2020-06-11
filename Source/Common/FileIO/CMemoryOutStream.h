#ifndef CMEMORYOUTSTREAM_H
#define CMEMORYOUTSTREAM_H

#include "IOutputStream.h"

class CMemoryOutStream : public IOutputStream
{
    char *mpDataStart = nullptr;
    uint32 mDataSize = 0;
    uint32 mPos = 0;
    uint32 mUsed = 0;

public:
    CMemoryOutStream();
    CMemoryOutStream(void *pData, uint32 Size, EEndian mDataEndianness);
    ~CMemoryOutStream();
    void SetData(void *pData, uint32 Size, EEndian mDataEndianness);

    void WriteBytes(const void* pkSrc, uint32 Count) override;
    bool Seek(int32 Offset, uint32 Origin) override;
    uint32 Tell() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32 Size() const override;
    uint32 SpaceUsed() const;
    void SetSize(uint32 Size);
    void* Data() const;
    void* DataAtPosition() const;
};

#endif // CMEMORYOUTSTREAM_H
