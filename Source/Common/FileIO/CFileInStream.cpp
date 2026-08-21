#include "CFileInStream.h"

#include <utility>

CFileInStream::CFileInStream() = default;

CFileInStream::CFileInStream(const TString& rkFile, std::endian FileEndianness)
{
    Open(rkFile, FileEndianness);
}

CFileInStream::~CFileInStream()
{
    if (IsValid())
        Close();
}

CFileInStream::CFileInStream(CFileInStream&& Other) noexcept
    : mpFStream{std::exchange(Other.mpFStream, nullptr)}
    , mName{std::exchange(Other.mName, TString())}
    , mFileSize{std::exchange(Other.mFileSize, 0)}
{
    mDataEndianness = Other.mDataEndianness;
    mDataSource = std::exchange(Other.mDataSource, TString());
}

CFileInStream& CFileInStream::operator=(CFileInStream&& Other) noexcept
{
    if (this == &Other)
        return *this;

    mpFStream = std::exchange(Other.mpFStream, nullptr);
    mName = std::exchange(Other.mName, TString());
    mFileSize = std::exchange(Other.mFileSize, 0);
    mDataEndianness = Other.mDataEndianness;
    mDataSource = std::exchange(Other.mDataSource, TString());
    return *this;
}

void CFileInStream::Open(const TString& rkFile, std::endian FileEndianness)
{
    if (IsValid())
        Close();

#ifdef _WIN32
    _wfopen_s(&mpFStream, ToWChar(rkFile), L"rb");
#else
    mpFStream = fopen(rkFile.data(), "rb");
#endif
    mName = rkFile;
    mDataEndianness = FileEndianness;

    if (IsValid())
    {
        Seek(0x0, SEEK_END);
        mFileSize = Tell();
        Seek(0x0, SEEK_SET);
    }
    else
    {
        mFileSize = 0;
    }

    SetSourceString(rkFile.GetFileName());
}

void CFileInStream::Close()
{
    if (IsValid())
        fclose(mpFStream);
    mpFStream = nullptr;
}

void CFileInStream::ReadBytes(void *pDst, uint32_t Count)
{
    if (!IsValid())
        return;

    fread(pDst, 1, Count, mpFStream);
}

bool CFileInStream::Seek(int32_t Offset, uint32_t Origin)
{
    if (!IsValid())
        return false;

    return (fseek(mpFStream, Offset, Origin) == 0);
}

bool CFileInStream::Seek64(int64_t Offset, uint32_t Origin)
{
    if (!IsValid())
        return false;

#ifdef _WIN32
    return (_fseeki64(mpFStream, Offset, Origin) == 0);
#else
    return (fseeko(mpFStream, Offset, Origin) == 0);
#endif
}

uint32_t CFileInStream::Tell() const
{
    if (!IsValid())
        return 0;

    return ftell(mpFStream);
}

uint64_t CFileInStream::Tell64() const
{
    if (!IsValid())
        return 0;

#ifdef _WIN32
    return _ftelli64(mpFStream);
#else
    return ftello(mpFStream);
#endif
}

bool CFileInStream::EoF() const
{
    return (Tell() >= mFileSize);
}

bool CFileInStream::IsValid() const
{
    return (mpFStream != 0);
}

uint32_t CFileInStream::Size() const
{
    return mFileSize;
}

TString CFileInStream::FileName() const
{
    return mName;
}
