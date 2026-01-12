#ifndef CBASICBINARYREADER
#define CBASICBINARYREADER

#include "Common/CFourCC.h"
#include "Common/FileIO/CFileInStream.h"
#include "Common/FileIO/CMemoryInStream.h"
#include "Common/FileIO/IInputStream.h"
#include "Common/Serialization/IArchive.h"
#include "Common/Serialization/CSerialVersion.h"

// This is a basic binary reader that doesn't do any checks on parameter names.
// This is the fastest serializer, but it relies entirely on parameter order so
// changes to file structure will break old versions of the file. Use carefully!
class CBasicBinaryReader : public IArchive
{
    IInputStream *mpStream = nullptr;
    bool mMagicValid = false;
    bool mOwnsStream = true;

public:
    explicit CBasicBinaryReader(const TString& rkFilename, uint32_t Magic)
    {
        mArchiveFlags = AF_Binary | AF_Reader | AF_NoSkipping;
        mpStream = new CFileInStream(rkFilename, std::endian::big);

        if (mpStream->IsValid())
        {
            mMagicValid = (mpStream->ReadU32() == Magic);
            SerializeVersion();
        }
    }

    explicit CBasicBinaryReader(IInputStream *pStream, const CSerialVersion& rkVersion)
        : mMagicValid(true)
        , mOwnsStream(false)
    {
        mArchiveFlags = AF_Binary | AF_Reader | AF_NoSkipping;

        ASSERT(pStream->IsValid());
        mpStream = pStream;
        SetVersion(rkVersion);
    }

    explicit CBasicBinaryReader(void *pData, uint32_t DataSize, const CSerialVersion& rkVersion, std::endian Endian = std::endian::native)
        : mMagicValid(true)
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

    bool ParamBegin(const char*, uint32_t) override { return true; }
    void ParamEnd() override {}

    bool PreSerializePointer(void*& Pointer, uint32_t Flags) override                     { return ArchiveVersion() >= eArVer_Refactor ? mpStream->ReadBool() : true; }
    virtual void SerializeContainerSize(uint32_t& rSize, const TString&, uint32_t Flags)  { SerializePrimitive(rSize, Flags); }
    void SerializeBulkData(void* pData, uint32_t Size, uint32_t Flags) override           { mpStream->ReadBytes(pData, Size); }

    void SerializePrimitive(bool& rValue, uint32_t Flags) override     { rValue = mpStream->ReadBool(); }
    void SerializePrimitive(char& rValue, uint32_t Flags) override     { rValue = mpStream->ReadS8(); }
    void SerializePrimitive(int8_t& rValue, uint32_t Flags) override   { rValue = mpStream->ReadS8(); }
    void SerializePrimitive(uint8_t& rValue, uint32_t Flags) override  { rValue = mpStream->ReadU8(); }
    void SerializePrimitive(int16_t& rValue, uint32_t Flags) override  { rValue = mpStream->ReadS16(); }
    void SerializePrimitive(uint16_t& rValue, uint32_t Flags) override { rValue = mpStream->ReadU16(); }
    void SerializePrimitive(int32_t& rValue, uint32_t Flags) override  { rValue = mpStream->ReadS32(); }
    void SerializePrimitive(uint32_t& rValue, uint32_t Flags) override { rValue = mpStream->ReadU32(); }
    void SerializePrimitive(int64_t& rValue, uint32_t Flags) override  { rValue = mpStream->ReadS64(); }
    void SerializePrimitive(uint64_t& rValue, uint32_t Flags) override { rValue = mpStream->ReadU64(); }
    void SerializePrimitive(float& rValue, uint32_t Flags) override    { rValue = mpStream->ReadF32(); }
    void SerializePrimitive(double& rValue, uint32_t Flags) override   { rValue = mpStream->ReadF64(); }
    void SerializePrimitive(TString& rValue, uint32_t Flags) override  { rValue = mpStream->ReadSizedString(); }
    void SerializePrimitive(CFourCC& rValue, uint32_t Flags) override  { rValue = CFourCC(*mpStream); }
    void SerializePrimitive(CAssetID& rValue, uint32_t Flags) override { rValue = CAssetID(*mpStream, mGame); }
};

#endif // CBASICBINARYREADER

