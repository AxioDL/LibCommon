#ifndef CBASICBINARYWRITER
#define CBASICBINARYWRITER

#include "Common/CFourCC.h"
#include "Common/FileIO/IOutputStream.h"
#include "Common/FileIO/CFileOutStream.h"
#include "Common/Serialization/IArchive.h"

// This is a basic binary reader that doesn't do any checks on parameter names.
// This is the fastest serializer, but it relies entirely on parameter order so
// changes to file structure will break old versions of the file. Use carefully!
class CBasicBinaryWriter : public IArchive
{
    IOutputStream *mpStream = nullptr;
    uint32_t mMagic = 0;
    bool mOwnsStream = true;

public:
    explicit CBasicBinaryWriter(const TString& rkFilename, uint32_t Magic, uint16 FileVersion, EGame Game)
        : mMagic(Magic)
    {
        mArchiveFlags = AF_Binary | AF_Writer | AF_NoSkipping;
        mpStream = new CFileOutStream(rkFilename, std::endian::big);

        if (mpStream->IsValid())
        {
            mpStream->WriteS32(0); // Magic is written after the rest of the file is successfully saved
            SetVersion(skCurrentArchiveVersion, FileVersion, Game);
            SerializeVersion();
        }
    }

    explicit CBasicBinaryWriter(IOutputStream *pStream, uint16 FileVersion, EGame Game)
        : mOwnsStream(false)
    {
        ASSERT(pStream->IsValid());
        mArchiveFlags = AF_Binary | AF_Writer | AF_NoSkipping;
        mpStream = pStream;
        SetVersion(skCurrentArchiveVersion, FileVersion, Game);
    }

    explicit CBasicBinaryWriter(IOutputStream *pStream, const CSerialVersion& rkVersion)
        : mOwnsStream(false)
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
            mpStream->WriteU32(mMagic);
            delete mpStream;
        }
    }

    bool IsValid() const { return mpStream->IsValid(); }

    // Interface
    virtual bool IsReader() const       { return false; }
    virtual bool IsWriter() const       { return true; }
    virtual bool IsTextFormat() const   { return false; }

    bool ParamBegin(const char*, uint32_t) override { return true; }
    void ParamEnd() override {}

    bool PreSerializePointer(void*& Pointer, uint32_t Flags) override
    {
        bool ValidPtr = (Pointer != nullptr);
        mpStream->WriteBool(ValidPtr);
        return ValidPtr;
    }

    virtual void SerializeContainerSize(uint32_t& rSize, const TString&)  { mpStream->WriteU32(rSize); }

    void SerializePrimitive(bool& rValue, uint32_t Flags) override     { mpStream->WriteBool(rValue); }
    void SerializePrimitive(char& rValue, uint32_t Flags) override     { mpStream->WriteS8(rValue); }
    void SerializePrimitive(int8_t& rValue, uint32_t Flags) override   { mpStream->WriteS8(rValue); }
    void SerializePrimitive(uint8_t& rValue, uint32_t Flags) override  { mpStream->WriteU8(rValue); }
    void SerializePrimitive(int16_t& rValue, uint32_t Flags) override  { mpStream->WriteS16(rValue); }
    void SerializePrimitive(uint16_t& rValue, uint32_t Flags) override { mpStream->WriteU16(rValue); }
    void SerializePrimitive(int32_t& rValue, uint32_t Flags) override  { mpStream->WriteS32(rValue); }
    void SerializePrimitive(uint32_t& rValue, uint32_t Flags) override { mpStream->WriteU32(rValue); }
    void SerializePrimitive(int64_t& rValue, uint32_t Flags) override  { mpStream->WriteS64(rValue); }
    void SerializePrimitive(uint64_t& rValue, uint32_t Flags) override { mpStream->WriteU64(rValue); }
    void SerializePrimitive(float& rValue, uint32_t Flags) override    { mpStream->WriteF32(rValue); }
    void SerializePrimitive(double& rValue, uint32_t Flags) override   { mpStream->WriteF64(rValue); }
    void SerializePrimitive(TString& rValue, uint32_t Flags) override  { mpStream->WriteSizedString(rValue); }
    void SerializePrimitive(CFourCC& rValue, uint32_t Flags) override  { rValue.Write(*mpStream); }
    void SerializePrimitive(CAssetID& rValue, uint32_t Flags) override { rValue.Write(*mpStream, CAssetID::GameIDLength(Game())); }
    void SerializeBulkData(void* pData, uint32_t Size, uint32_t Flags) override { mpStream->WriteBytes(pData, Size); }
};

#endif // CBASICBINARYWRITER

