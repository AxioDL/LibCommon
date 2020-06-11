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
    IInputStream *mpStream = nullptr;
    bool mMagicValid = false;
    bool mOwnsStream = true;

public:
    CBasicBinaryReader(const TString& rkFilename, uint32 Magic)
        : IArchive()
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
    {
        mArchiveFlags = AF_Binary | AF_Reader | AF_NoSkipping;
        mpStream = new CMemoryInStream(pData, DataSize, Endian);
        SetVersion(rkVersion);
    }

    ~CBasicBinaryReader() override
    {
        if (mOwnsStream)
            delete mpStream;
    }

    bool IsValid() const { return mpStream->IsValid(); }

    // Interface
    virtual bool IsReader() const       { return true; }
    virtual bool IsWriter() const       { return false; }
    virtual bool IsTextFormat() const   { return false; }

    bool ParamBegin(const char*, uint32) override { return true; }
    void ParamEnd() override {}

    bool PreSerializePointer(void*& Pointer, uint32 Flags) override                     { return ArchiveVersion() >= eArVer_Refactor ? mpStream->ReadBool() : true; }
    virtual void SerializeContainerSize(uint32& rSize, const TString&, uint32 Flags)    { SerializePrimitive(rSize, Flags); }
    void SerializeBulkData(void* pData, uint32 Size, uint32 Flags) override             { mpStream->ReadBytes(pData, Size); }

    void SerializePrimitive(bool& rValue, uint32 Flags) override { rValue = mpStream->ReadBool(); }
    void SerializePrimitive(char& rValue, uint32 Flags) override { rValue = mpStream->ReadByte(); }
    void SerializePrimitive(int8& rValue, uint32 Flags) override { rValue = mpStream->ReadByte(); }
    void SerializePrimitive(uint8& rValue, uint32 Flags) override { rValue = mpStream->ReadByte(); }
    void SerializePrimitive(int16& rValue, uint32 Flags) override { rValue = mpStream->ReadShort(); }
    void SerializePrimitive(uint16& rValue, uint32 Flags) override { rValue = mpStream->ReadShort(); }
    void SerializePrimitive(int32& rValue, uint32 Flags) override { rValue = mpStream->ReadLong(); }
    void SerializePrimitive(uint32& rValue, uint32 Flags) override { rValue = mpStream->ReadLong(); }
    void SerializePrimitive(int64& rValue, uint32 Flags) override { rValue = mpStream->ReadLongLong(); }
    void SerializePrimitive(uint64& rValue, uint32 Flags) override { rValue = mpStream->ReadLongLong(); }
    void SerializePrimitive(float& rValue, uint32 Flags) override { rValue = mpStream->ReadFloat(); }
    void SerializePrimitive(double& rValue, uint32 Flags) override { rValue = mpStream->ReadDouble(); }
    void SerializePrimitive(TString& rValue, uint32 Flags) override { rValue = mpStream->ReadSizedString(); }
    void SerializePrimitive(CFourCC& rValue, uint32 Flags) override { rValue = CFourCC(*mpStream); }
    void SerializePrimitive(CAssetID& rValue, uint32 Flags) override { rValue = CAssetID(*mpStream, mGame); }
};

#endif // CBASICBINARYREADER

