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
    IOutputStream *mpStream;
    uint32 mMagic;
    bool mOwnsStream;

public:
    CBasicBinaryWriter(const TString& rkFilename, uint32 Magic, uint16 FileVersion, EGame Game)
        : IArchive()
        , mMagic(Magic)
        , mOwnsStream(true)
    {
        mArchiveFlags = AF_Binary | AF_Writer | AF_NoSkipping;
        mpStream = new CFileOutStream(rkFilename, IOUtil::eBigEndian);

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

    ~CBasicBinaryWriter()
    {
        // Write magic and delete stream
        if (mOwnsStream)
        {
            mpStream->GoTo(0);
            mpStream->WriteLong(mMagic);
            delete mpStream;
        }
    }

    inline bool IsValid() const { return mpStream->IsValid(); }

    // Interface
    virtual bool IsReader() const       { return false; }
    virtual bool IsWriter() const       { return true; }
    virtual bool IsTextFormat() const   { return false; }

    virtual bool ParamBegin(const char*, uint32)    { return true; }
    virtual void ParamEnd()                         { }

    virtual bool PreSerializePointer(void*& Pointer, uint32 Flags)
    {
        bool ValidPtr = (Pointer != nullptr);
        mpStream->WriteBool(ValidPtr);
        return ValidPtr;
    }

    virtual void SerializeContainerSize(uint32& rSize, const TString&)  { mpStream->WriteLong(rSize); }

    virtual void SerializePrimitive(bool& rValue, uint32 Flags)             { mpStream->WriteBool(rValue); }
    virtual void SerializePrimitive(char& rValue, uint32 Flags)             { mpStream->WriteByte(rValue); }
    virtual void SerializePrimitive(int8& rValue, uint32 Flags)             { mpStream->WriteByte(rValue); }
    virtual void SerializePrimitive(uint8& rValue, uint32 Flags)            { mpStream->WriteByte(rValue); }
    virtual void SerializePrimitive(int16& rValue, uint32 Flags)            { mpStream->WriteShort(rValue); }
    virtual void SerializePrimitive(uint16& rValue, uint32 Flags)           { mpStream->WriteShort(rValue); }
    virtual void SerializePrimitive(int32& rValue, uint32 Flags)            { mpStream->WriteLong(rValue); }
    virtual void SerializePrimitive(uint32& rValue, uint32 Flags)           { mpStream->WriteLong(rValue); }
    virtual void SerializePrimitive(int64& rValue, uint32 Flags)            { mpStream->WriteLongLong(rValue); }
    virtual void SerializePrimitive(uint64& rValue, uint32 Flags)           { mpStream->WriteLongLong(rValue); }
    virtual void SerializePrimitive(float& rValue, uint32 Flags)            { mpStream->WriteFloat(rValue); }
    virtual void SerializePrimitive(double& rValue, uint32 Flags)           { mpStream->WriteDouble(rValue); }
    virtual void SerializePrimitive(TString& rValue, uint32 Flags)          { mpStream->WriteSizedString(rValue); }
    virtual void SerializePrimitive(TWideString& rValue, uint32 Flags)      { mpStream->WriteSizedWString(rValue); }
    virtual void SerializePrimitive(CFourCC& rValue, uint32 Flags)          { rValue.Write(*mpStream); }
    virtual void SerializePrimitive(CAssetID& rValue, uint32 Flags)         { rValue.Write(*mpStream, CAssetID::GameIDLength(Game())); }
    virtual void SerializeBulkData(void* pData, uint32 Size, uint32 Flags)  { mpStream->WriteBytes(pData, Size); }
};

#endif // CBASICBINARYWRITER

