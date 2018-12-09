#ifndef CMEMORYOUTSTREAM_H
#define CMEMORYOUTSTREAM_H

#include "IOutputStream.h"

class CMemoryOutStream : public IOutputStream
{
    char *mpDataStart;
    uint32 mDataSize;
    uint32 mPos;
    uint32 mUsed;

public:
    CMemoryOutStream();
    CMemoryOutStream(void *pData, uint32 Size, IOUtil::EEndianness mDataEndianness);
    ~CMemoryOutStream();
    void SetData(void *pData, uint32 Size, IOUtil::EEndianness mDataEndianness);

    void WriteBytes(const void *pkSrc, uint32 Count);
    bool Seek(int32 Offset, uint32 Origin);
    uint32 Tell() const;
    bool EoF() const;
    bool IsValid() const;
    uint32 Size() const;
    uint32 SpaceUsed() const;
    void SetSize(uint32 Size);
    void* Data() const;
    void* DataAtPosition() const;
};

#endif // CMEMORYOUTSTREAM_H
