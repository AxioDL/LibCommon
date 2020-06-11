#ifndef CBASICBINARYWRITER
#define CBASICBINARYWRITER

#include "IArchive.h"
#include "Common/CFourCC.h"
#include "Common/FileIO/IOutputStream.h"

// This is a basic binary reader that doesn't do any checks on parameter names.
// This is the fastest serializer, but it relies entirely on parameter order so
// changes to file structure will break old versions of the file. Use carefully!
class CBasicBinaryWriter : public IArchive
{
    IOutputStream *mpStream = nullptr;
    uint32 mMagic = 0;
    bool mOwnsStream = true;

public:
    CBasicBinaryWriter(const TString& rkFilename, uint32 Magic, uint16 FileVersion, EGame Game)
        : IArchive()
        , mMagic(Magic)
    {
        mArchiveFlags = AF_Binary | AF_Writer | AF_NoSkipping;
        mpStream = new CFileOutStream(rkFilename, EEndian::BigEndian);

        if (mpStream->IsValid())
        {
            mpStream->WriteLong(0); // Magic is written after the rest of the file is successfully saved
            SetVersion(skCurrentArchiveVersion, FileVersion, Game);
            SerializeVersion();
        }
    }

    CBasicBinaryWriter(IOutputStream *pStream, uint16 FileVersion, EGame Game)
        : IArchive()
        , mOwnsStream(false)
    {
        ASSERT(pStream->IsValid());
        mArchiveFlags = AF_Binary | AF_Writer | AF_NoSkipping;
        mpStream = pStream;
        SetVersion(skCurrentArchiveVersion, FileVersion, Game);
    }

    CBasicBinaryWriter(IOutputStream *pStream, const CSerialVersion& rkVersion)
        : IArchive()
        , mOwnsStream(false)
    {
        ASSERT(pStream->IsValid());
        mArchiveFlags = AF_Binary | AF_Writer | AF_NoSkipping;
        mpStream = pStream;
        SetVersion(rkVersion);
    }

    ~CBasicBinaryWriter() override
    {
        // Write magic and delete stream
        if (mOwnsStream)
        {
            mpStream->GoTo(0);
            mpStream->WriteLong(mMagic);
            delete mpStream;
        }
    }

    bool IsValid() const { return mpStream->IsValid(); }

    // Interface
    virtual bool IsReader() const       { return false; }
    virtual bool IsWriter() const       { return true; }
    virtual bool IsTextFormat() const   { return false; }

    bool ParamBegin(const char*, uint32) override { return true; }
    void ParamEnd() override {}

    bool PreSerializePointer(void*& Pointer, uint32 Flags) override
    {
        bool ValidPtr = (Pointer != nullptr);
        mpStream->WriteBool(ValidPtr);
        return ValidPtr;
    }

    virtual void SerializeContainerSize(uint32& rSize, const TString&)  { mpStream->WriteLong(rSize); }

    void SerializePrimitive(bool& rValue, uint32 Flags) override { mpStream->WriteBool(rValue); }
    void SerializePrimitive(char& rValue, uint32 Flags) override { mpStream->WriteByte(rValue); }
    void SerializePrimitive(int8& rValue, uint32 Flags) override { mpStream->WriteByte(rValue); }
    void SerializePrimitive(uint8& rValue, uint32 Flags) override { mpStream->WriteByte(rValue); }
    void SerializePrimitive(int16& rValue, uint32 Flags) override { mpStream->WriteShort(rValue); }
    void SerializePrimitive(uint16& rValue, uint32 Flags) override { mpStream->WriteShort(rValue); }
    void SerializePrimitive(int32& rValue, uint32 Flags) override { mpStream->WriteLong(rValue); }
    void SerializePrimitive(uint32& rValue, uint32 Flags) override { mpStream->WriteLong(rValue); }
    void SerializePrimitive(int64& rValue, uint32 Flags) override { mpStream->WriteLongLong(rValue); }
    void SerializePrimitive(uint64& rValue, uint32 Flags) override { mpStream->WriteLongLong(rValue); }
    void SerializePrimitive(float& rValue, uint32 Flags) override { mpStream->WriteFloat(rValue); }
    void SerializePrimitive(double& rValue, uint32 Flags) override { mpStream->WriteDouble(rValue); }
    void SerializePrimitive(TString& rValue, uint32 Flags) override { mpStream->WriteSizedString(rValue); }
    void SerializePrimitive(CFourCC& rValue, uint32 Flags) override { rValue.Write(*mpStream); }
    void SerializePrimitive(CAssetID& rValue, uint32 Flags) override { rValue.Write(*mpStream, CAssetID::GameIDLength(Game())); }
    void SerializeBulkData(void* pData, uint32 Size, uint32 Flags) override { mpStream->WriteBytes(pData, Size); }
};

#endif // CBASICBINARYWRITER

