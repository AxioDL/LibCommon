#include "CFileOutStream.h"

CFileOutStream::CFileOutStream() = default;

CFileOutStream::CFileOutStream(const TString& rkFile)
{
    Open(rkFile, EEndian::BigEndian);
}

CFileOutStream::CFileOutStream(const TString& rkFile, EEndian FileEndianness)
{
    Open(rkFile, FileEndianness);
}

CFileOutStream::CFileOutStream(const CFileOutStream& rkSrc)
{
    Open(rkSrc.mName, rkSrc.mDataEndianness);

    if (rkSrc.IsValid())
        Seek(rkSrc.Tell(), SEEK_SET);
}

CFileOutStream::~CFileOutStream()
{
    if (IsValid())
        Close();
}

void CFileOutStream::Open(const TString& rkFile, EEndian FileEndianness)
{
    if (IsValid())
        Close();

#ifdef _WIN32
    _wfopen_s(&mpFStream, ToWChar(rkFile), L"wb");
#else
    mpFStream = fopen(*rkFile, "wb");
#endif
    mName = rkFile;
    mDataEndianness = FileEndianness;
    mSize = 0;
}

void CFileOutStream::Update(const TString& rkFile, EEndian FileEndianness)
{
    if (IsValid())
        Close();

#ifdef _WIN32
    _wfopen_s(&mpFStream, ToWChar(rkFile), L"rb+");
#else
    mpFStream = fopen(*rkFile, "rb+");
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

void CFileOutStream::WriteBytes(const void *pkSrc, uint32 Count)
{
    if (!IsValid()) return;
    fwrite(pkSrc, 1, Count, mpFStream);
    if (Tell() > mSize) mSize = Tell();
}

bool CFileOutStream::Seek(int32 Offset, uint32 Origin)
{
    if (!IsValid()) return false;
    return (fseek(mpFStream, Offset, Origin) != 0);
}

bool CFileOutStream::Seek64(int64 Offset, uint32 Origin)
{
    if (!IsValid()) return false;
#ifdef _WIN32
    return (_fseeki64(mpFStream, Offset, Origin) != 0);
#else
    return (fseeko(mpFStream, Offset, Origin) != 0);
#endif
}

uint32 CFileOutStream::Tell() const
{
    if (!IsValid()) return 0;
    return ftell(mpFStream);
}

uint64 CFileOutStream::Tell64() const
{
    if (!IsValid()) return 0;
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

uint32 CFileOutStream::Size() const
{
    if (!IsValid()) return 0;
    return mSize;
}

TString CFileOutStream::FileName() const
{
    return mName;
}
