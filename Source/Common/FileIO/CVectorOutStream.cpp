#include "CVectorOutStream.h"
#include "Common/Common.h"

CVectorOutStream::CVectorOutStream()
    : mpVector(new std::vector<char>)
{
    mDataEndianness = EEndian::BigEndian;
}

CVectorOutStream::CVectorOutStream(EEndian DataEndianness)
    : mpVector(new std::vector<char>)
{
    mDataEndianness = DataEndianness;
}

CVectorOutStream::CVectorOutStream(uint32 InitialSize, EEndian DataEndianness)
    : mpVector(new std::vector<char>(InitialSize))
{
    mDataEndianness = DataEndianness;
}

CVectorOutStream::CVectorOutStream(std::vector<char> *pVector, EEndian DataEndianness)
    : mpVector(pVector)
    , mOwnsVector(false)
{
    mDataEndianness = DataEndianness;
}

CVectorOutStream::~CVectorOutStream()
{
    if (mOwnsVector) delete mpVector;
}

void CVectorOutStream::WriteBytes(const void *pkSrc, uint32 Count)
{
    if (!IsValid()) return;

    uint32 NewSize = mPos + Count;

    if (NewSize > mpVector->size())
    {
        if (NewSize > mpVector->capacity())
            mpVector->reserve( VAL_ALIGN(mPos + Count, skAllocSize) );

        mpVector->resize(NewSize);
    }

    memcpy(mpVector->data() + mPos, pkSrc, Count);
    mPos += Count;
}

bool CVectorOutStream::Seek(int32 Offset, uint32 Origin)
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
            mPos = mpVector->size() - Offset;
            break;

        default:
            return false;
    }

    if (mPos < 0)
    {
        mPos = 0;
        return false;
    }

    if (mPos > mpVector->size())
        mpVector->resize(mPos);

    return true;
}

uint32 CVectorOutStream::Tell() const
{
    return mPos;
}

bool CVectorOutStream::EoF() const
{
    return false;
}

bool CVectorOutStream::IsValid() const
{
    return true;
}

uint32 CVectorOutStream::Size() const
{
    return mPos;
}

void CVectorOutStream::SetVector(std::vector<char> *pVector)
{
    if (mOwnsVector)
    {
        delete mpVector;
        mOwnsVector = false;
    }

    mpVector = pVector;
    mPos = 0;
}

void* CVectorOutStream::Data()
{
    return mpVector->data();
}

void* CVectorOutStream::DataAtPosition()
{
    return mpVector->data() + mPos;
}

void CVectorOutStream::Clear()
{
    mPos = 0;
    mpVector->clear();
}
