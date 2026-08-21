#include "CFileOutStream.h"

#include <utility>

CFileOutStream::CFileOutStream() = default;

CFileOutStream::CFileOutStream(const TString& rkFile, std::endian FileEndianness)
{
    Open(rkFile, FileEndianness);
}

CFileOutStream::~CFileOutStream()
{
    if (IsValid())
        Close();
}

CFileOutStream::CFileOutStream(CFileOutStream&& Other) noexcept
    : mpFStream{std::exchange(Other.mpFStream, nullptr)}
    , mName{std::exchange(Other.mName, TString())}
    , mSize{std::exchange(Other.mSize, 0)}
{
    mDataEndianness = Other.mDataEndianness;
}

CFileOutStream& CFileOutStream::operator=(CFileOutStream&& Other) noexcept
{
    if (this == &Other)
        return *this;

    mpFStream = std::exchange(Other.mpFStream, nullptr);
    mName = std::exchange(Other.mName, TString());
    mSize = std::exchange(Other.mSize, 0);
    mDataEndianness = Other.mDataEndianness;
    return *this;
}

void CFileOutStream::Open(const TString& rkFile, std::endian FileEndianness)
{
    if (IsValid())
        Close();

#ifdef _WIN32
    _wfopen_s(&mpFStream, ToWChar(rkFile), L"wb");
#else
    mpFStream = fopen(rkFile.data(), "wb");
#endif
    mName = rkFile;
    mDataEndianness = FileEndianness;
    mSize = 0;
}

void CFileOutStream::Update(const TString& rkFile, std::endian FileEndianness)
{
    if (IsValid())
        Close();

#ifdef _WIN32
    _wfopen_s(&mpFStream, ToWChar(rkFile), L"rb+");
#else
    mpFStream = fopen(rkFile.data(), "rb+");
#endif
    mName = rkFile;
    mDataEndianness = FileEndianness;
    Seek(0x0, SEEK_END);
    mSize = Tell();
    Seek(0x0, SEEK_SET);
}

void CFileOutStream::Close()
{
    if (IsValid())
        fclose(mpFStream);
    mpFStream = nullptr;
    mSize = 0;
}

void CFileOutStream::WriteBytes(const void *pkSrc, uint32_t Count)
{
    if (!IsValid())
        return;

    fwrite(pkSrc, 1, Count, mpFStream);

    if (Tell() > mSize)
        mSize = Tell();
}

bool CFileOutStream::Seek(int32_t Offset, uint32_t Origin)
{
    if (!IsValid())
        return false;

    return (fseek(mpFStream, Offset, Origin) == 0);
}

bool CFileOutStream::Seek64(int64_t Offset, uint32_t Origin)
{
    if (!IsValid())
        return false;

#ifdef _WIN32
    return (_fseeki64(mpFStream, Offset, Origin) == 0);
#else
    return (fseeko(mpFStream, Offset, Origin) == 0);
#endif
}

uint32_t CFileOutStream::Tell() const
{
    if (!IsValid())
        return 0;

    return ftell(mpFStream);
}

uint64_t CFileOutStream::Tell64() const
{
    if (!IsValid())
        return 0;

#ifdef _WIN32
    return _ftelli64(mpFStream);
#else
    return ftello(mpFStream);
#endif
}

bool CFileOutStream::EoF() const
{
    return (Tell() == Size());
}

bool CFileOutStream::IsValid() const
{
    return (mpFStream != 0);
}

uint32_t CFileOutStream::Size() const
{
    if (!IsValid())
        return 0;

    return mSize;
}

TString CFileOutStream::FileName() const
{
    return mName;
}
