#include "CFileInStream.h"

CFileInStream::CFileInStream()
    : mpFStream(nullptr)
{
}

CFileInStream::CFileInStream(const TString& rkFile)
    : mpFStream(nullptr)
{
    Open(rkFile, EEndian::BigEndian);
}

CFileInStream::CFileInStream(const TString& rkFile, EEndian FileEndianness)
    : mpFStream(nullptr)
{
    Open(rkFile, FileEndianness);
}

CFileInStream::CFileInStream(const CFileInStream& rkSrc)
    : mpFStream(nullptr)
{
    Open(rkSrc.mName, rkSrc.mDataEndianness);

    if (rkSrc.IsValid())
        Seek(rkSrc.Tell(), SEEK_SET);
}

CFileInStream::~CFileInStream()
{
    if (IsValid())
        Close();
}

void CFileInStream::Open(const TString& rkFile, EEndian FileEndianness)
{
    if (IsValid())
        Close();

    _wfopen_s(&mpFStream, ToWChar(rkFile), L"rb");
    mName = rkFile;
    mDataEndianness = FileEndianness;

    if (IsValid())
    {
        Seek(0x0, SEEK_END);
        mFileSize = Tell();
        Seek(0x0, SEEK_SET);
    }
    else
        mFileSize = 0;

    SetSourceString(rkFile.GetFileName());
}

void CFileInStream::Close()
{
    if (IsValid())
        fclose(mpFStream);
    mpFStream = nullptr;
}

void CFileInStream::ReadBytes(void *pDst, uint32 Count)
{
    if (!IsValid()) return;
    fread(pDst, 1, Count, mpFStream);
}

bool CFileInStream::Seek(int32 Offset, uint32 Origin)
{
    if (!IsValid()) return false;
    return (fseek(mpFStream, Offset, Origin) != 0);
}

bool CFileInStream::Seek64(int64 Offset, uint32 Origin)
{
    if (!IsValid()) return false;
    return (_fseeki64(mpFStream, Offset, Origin) != 0);
}

uint32 CFileInStream::Tell() const
{
    if (!IsValid()) return 0;
    return ftell(mpFStream);
}

uint64 CFileInStream::Tell64() const
{
    if (!IsValid()) return 0;
    return _ftelli64(mpFStream);
}

bool CFileInStream::EoF() const
{
    return (Tell() >= mFileSize);
}

bool CFileInStream::IsValid() const
{
    return (mpFStream != 0);
}

uint32 CFileInStream::Size() const
{
    return mFileSize;
}

TString CFileInStream::FileName() const
{
    return mName;
}
