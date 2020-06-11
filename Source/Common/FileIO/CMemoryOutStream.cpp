#include "CMemoryOutStream.h"

CMemoryOutStream::CMemoryOutStream() = default;

CMemoryOutStream::CMemoryOutStream(void *pData, uint32 Size, EEndian DataEndianness)
{
    SetData(pData, Size, DataEndianness);
}

CMemoryOutStream::~CMemoryOutStream() = default;

void CMemoryOutStream::SetData(void *pData, uint32 Size, EEndian DataEndianness)
{
    mpDataStart = (char*) pData;
    mDataSize = Size;
    mPos = 0;
    mUsed = 0;
    mDataEndianness = DataEndianness;
}

void CMemoryOutStream::WriteBytes(const void *pkSrc, uint32 Count)
{
    if (!IsValid()) return;

    memcpy(mpDataStart + mPos, pkSrc, Count);
    mPos += Count;
    if (mPos > mUsed) mUsed = mPos;
}

bool CMemoryOutStream::Seek(int32 Offset, uint32 Origin)
{
    if (!IsValid()) return false;

    switch (Origin)
    {
        case SEEK_SET:
            mPos = Offset;
            break;

        case SEEK_CUR:
            mPos += Offset;
            break;

        case SEEK_END:
            mPos = mDataSize - Offset;
            break;

        default:
            return false;
    }

    if (mPos < 0)
    {
        mPos = 0;
        return false;
    }

    if (mPos > mDataSize)
    {
        mPos = mDataSize;
        return false;
    }

    return true;
}

uint32 CMemoryOutStream::Tell() const
{
    return mPos;
}

bool CMemoryOutStream::EoF() const
{
    return (mPos >= mDataSize);
}

bool CMemoryOutStream::IsValid() const
{
    return (mpDataStart != nullptr);
}

uint32 CMemoryOutStream::Size() const
{
    return mDataSize;
}

uint32 CMemoryOutStream::SpaceUsed() const
{
    return mUsed;
}

void CMemoryOutStream::SetSize(uint32 Size)
{
    mDataSize = Size;
    if (mPos > mDataSize)
        mPos = mDataSize;
}

void* CMemoryOutStream::Data() const
{
    return mpDataStart;
}

void* CMemoryOutStream::DataAtPosition() const
{
    return mpDataStart + mPos;
}
