#ifndef CBINARYWRITER
#define CBINARYWRITER

#include "Common/CFourCC.h"
#include "Common/FileIO/CFileOutStream.h"
#include "Common/Serialization/IArchive.h"

class CBinaryWriter : public IArchive
{
    struct SParameter
    {
        uint32_t Offset;
        uint32_t NumSubParams;
    };
    std::vector<SParameter> mParamStack;

    IOutputStream *mpStream = nullptr;
    uint32_t mMagic = 0;
    bool mOwnsStream = false;

public:
    explicit CBinaryWriter(const TString& rkFilename, uint32_t Magic, uint16_t FileVersion = 0, EGame Game = EGame::Invalid)
        : mMagic(Magic)
        , mOwnsStream(true)
    {
        mArchiveFlags = AF_Writer | AF_Binary;
        mpStream = new CFileOutStream(rkFilename, std::endian::big);

        if (mpStream->IsValid())
        {
            mpStream->WriteLong(0); // Magic is written after the rest of the file has been successfully written
            SetVersion(skCurrentArchiveVersion, FileVersion, Game);
        }

        mpStream->WriteShort(mArchiveVersion);
        mpStream->WriteShort(mFileVersion);
        mpStream->WriteFourCC(GameTo4CC(mGame).ToU32());

        InitParamStack();
    }

    explicit CBinaryWriter(IOutputStream *pStream, uint16_t FileVersion = 0, EGame Game = EGame::Invalid)
    {
        ASSERT(pStream && pStream->IsValid());
        mArchiveFlags = AF_Writer | AF_Binary;
        mpStream = pStream;
        SetVersion(skCurrentArchiveVersion, FileVersion, Game);
        InitParamStack();
    }

    explicit CBinaryWriter(IOutputStream *pStream, const CSerialVersion& rkVersion)
    {
        ASSERT(pStream && pStream->IsValid());
        mArchiveFlags = AF_Writer | AF_Binary;
        mpStream = pStream;
        SetVersion(rkVersion);
        InitParamStack();
    }

    ~CBinaryWriter() override
    {
        // Ensure all params have been finished
        ASSERT(mParamStack.size() == 1);

        // Finish root param
        ParamEnd();

        // Write magic and delete stream
        if (mOwnsStream)
        {
            mpStream->GoTo(0);
            mpStream->WriteLong(mMagic);
            delete mpStream;
        }
    }

    bool IsValid() const { return mpStream->IsValid(); }

private:
    void InitParamStack()
    {
        mParamStack.reserve(20);
        mpStream->WriteLong(0xFFFFFFFF);
        mpStream->WriteLong(0); // Size filler
        mParamStack.push_back(SParameter{mpStream->Tell(), 0});
    }

public:
    // Interface
    bool ParamBegin(const char* pkName, uint32_t Flags) override
    {
        // Update parent param
        mParamStack.back().NumSubParams++;

        if (mParamStack.back().NumSubParams == 1)
            mpStream->WriteLong(-1); // Sub-param count filler

        // Write param metadata
        uint32_t ParamID = TString(pkName).Hash32();
        mpStream->WriteLong(ParamID);
        mpStream->WriteLong(-1); // Param size filler

        // Add new param to the stack
        mParamStack.push_back(SParameter{mpStream->Tell(), 0});

        return true;
    }

    void ParamEnd() override
    {
        // Write param size
        SParameter& rParam = mParamStack.back();
        uint32_t StartOffset = rParam.Offset;
        uint32_t EndOffset = mpStream->Tell();
        uint32_t ParamSize = (EndOffset - StartOffset);

        mpStream->GoTo(StartOffset - 4);
        mpStream->WriteLong(ParamSize);

        // Write param child count
        if (rParam.NumSubParams > 0 || mParamStack.size() == 1)
        {
            mpStream->WriteLong(rParam.NumSubParams);
        }

        mpStream->GoTo(EndOffset);
        mParamStack.pop_back();
    }

    bool PreSerializePointer(void*& Pointer, uint32_t Flags) override
    {
        bool ValidPtr = (Pointer != nullptr);
        *this << SerialParameter("PointerValid", ValidPtr);
        return ValidPtr;
    }

    virtual void SerializeContainerSize(uint32_t& rSize, const TString& /*rkElemName*/)
    {
        // Normally handled by ParamBegin and ParamEnd but we need to do something here to account for zero-sized containers
        if (rSize == 0)
            mpStream->WriteLong(0);
    }

    void SerializePrimitive(bool& rValue, uint32_t Flags) override { mpStream->WriteBool(rValue); }
    void SerializePrimitive(char& rValue, uint32_t Flags) override { mpStream->WriteByte(rValue); }
    void SerializePrimitive(int8_t& rValue, uint32_t Flags) override { mpStream->WriteByte(rValue); }
    void SerializePrimitive(uint8_t& rValue, uint32_t Flags) override { mpStream->WriteByte(rValue); }
    void SerializePrimitive(int16_t& rValue, uint32_t Flags) override { mpStream->WriteShort(rValue); }
    void SerializePrimitive(uint16_t& rValue, uint32_t Flags) override { mpStream->WriteShort(rValue); }
    void SerializePrimitive(int32_t& rValue, uint32_t Flags) override { mpStream->WriteLong(rValue); }
    void SerializePrimitive(uint32_t& rValue, uint32_t Flags) override { mpStream->WriteLong(rValue); }
    void SerializePrimitive(int64_t& rValue, uint32_t Flags) override { mpStream->WriteLongLong(rValue); }
    void SerializePrimitive(uint64_t& rValue, uint32_t Flags) override { mpStream->WriteLongLong(rValue); }
    void SerializePrimitive(float& rValue, uint32_t Flags) override { mpStream->WriteFloat(rValue); }
    void SerializePrimitive(double& rValue, uint32_t Flags) override { mpStream->WriteDouble(rValue); }
    void SerializePrimitive(TString& rValue, uint32_t Flags) override { mpStream->WriteSizedString(rValue); }
    void SerializePrimitive(CFourCC& rValue, uint32_t Flags) override { rValue.Write(*mpStream); }
    void SerializePrimitive(CAssetID& rValue, uint32_t Flags) override { rValue.Write(*mpStream, CAssetID::GameIDLength(Game())); }
    void SerializeBulkData(void* pData, uint32_t Size, uint32_t Flags) override { mpStream->WriteBytes(pData, Size); }
};

#endif // CBINARYWRITER

