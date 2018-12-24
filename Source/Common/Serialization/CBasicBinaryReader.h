#ifndef CBASICBINARYREADER
#define CBASICBINARYREADER

#include "IArchive.h"
#include "CSerialVersion.h"
#include "Common/CFourCC.h"
#include "Common/FileIO/IInputStream.h"

// This is a basic binary reader that doesn't do any checks on parameter names.
// This is the fastest serializer, but it relies entirely on parameter order so
// changes to file structure will break old versions of the file. Use carefully!
class CBasicBinaryReader : public IArchive
{
    IInputStream *mpStream;
    bool mMagicValid;
    bool mOwnsStream;

public:
    CBasicBinaryReader(const TString& rkFilename, uint32 Magic)
        : IArchive()
        , mOwnsStream(true)
    {
        mArchiveFlags = AF_Binary | AF_Reader | AF_NoSkipping;
        mpStream = new CFileInStream(rkFilename, EEndian::BigEndian);

        if (mpStream->IsValid())
        {
            mMagicValid = (mpStream->ReadLong() == Magic);
            SerializeVersion();
        }
    }

    CBasicBinaryReader(IInputStream *pStream, const CSerialVersion& rkVersion)
        : IArchive()
        , mMagicValid(true)
        , mOwnsStream(false)
    {
        mArchiveFlags = AF_Binary | AF_Reader | AF_NoSkipping;

        ASSERT(pStream->IsValid());
        mpStream = pStream;
        SetVersion(rkVersion);
    }

    CBasicBinaryReader(void *pData, uint32 DataSize, const CSerialVersion& rkVersion, EEndian Endian = EEndian::SystemEndian)
        : IArchive()
        , mMagicValid(true)
        , mOwnsStream(true)
    {
        mArchiveFlags = AF_Binary | AF_Reader | AF_NoSkipping;
        mpStream = new CMemoryInStream(pData, DataSize, Endian);
        SetVersion(rkVersion);
    }

    ~CBasicBinaryReader()
    {
        if (mOwnsStream) delete mpStream;
    }

    inline bool IsValid() const { return mpStream->IsValid(); }

    // Interface
    virtual bool IsReader() const       { return true; }
    virtual bool IsWriter() const       { return false; }
    virtual bool IsTextFormat() const   { return false; }

    virtual bool ParamBegin(const char*, uint32)    { return true; }
    virtual void ParamEnd()                         { }

    virtual bool PreSerializePointer(void*& Pointer, uint32 Flags)                      { return ArchiveVersion() >= eArVer_Refactor ? mpStream->ReadBool() : true; }
    virtual void SerializeContainerSize(uint32& rSize, const TString&, uint32 Flags)    { SerializePrimitive(rSize, Flags); }
    virtual void SerializeBulkData(void* pData, uint32 Size, uint32 Flags)              { mpStream->ReadBytes(pData, Size); }

    virtual void SerializePrimitive(bool& rValue, uint32 Flags)         { rValue = mpStream->ReadBool(); }
    virtual void SerializePrimitive(char& rValue, uint32 Flags)         { rValue = mpStream->ReadByte(); }
    virtual void SerializePrimitive(int8& rValue, uint32 Flags)         { rValue = mpStream->ReadByte(); }
    virtual void SerializePrimitive(uint8& rValue, uint32 Flags)        { rValue = mpStream->ReadByte(); }
    virtual void SerializePrimitive(int16& rValue, uint32 Flags)        { rValue = mpStream->ReadShort(); }
    virtual void SerializePrimitive(uint16& rValue, uint32 Flags)       { rValue = mpStream->ReadShort(); }
    virtual void SerializePrimitive(int32& rValue, uint32 Flags)        { rValue = mpStream->ReadLong(); }
    virtual void SerializePrimitive(uint32& rValue, uint32 Flags)       { rValue = mpStream->ReadLong(); }
    virtual void SerializePrimitive(int64& rValue, uint32 Flags)        { rValue = mpStream->ReadLongLong(); }
    virtual void SerializePrimitive(uint64& rValue, uint32 Flags)       { rValue = mpStream->ReadLongLong(); }
    virtual void SerializePrimitive(float& rValue, uint32 Flags)        { rValue = mpStream->ReadFloat(); }
    virtual void SerializePrimitive(double& rValue, uint32 Flags)       { rValue = mpStream->ReadDouble(); }
    virtual void SerializePrimitive(TString& rValue, uint32 Flags)      { rValue = mpStream->ReadSizedString(); }
    virtual void SerializePrimitive(CFourCC& rValue, uint32 Flags)      { rValue = CFourCC(*mpStream); }
    virtual void SerializePrimitive(CAssetID& rValue, uint32 Flags)     { rValue = CAssetID(*mpStream, mGame); }
};

#endif // CBASICBINARYREADER

