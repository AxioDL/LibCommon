#ifndef AXIO_CBINARYWRITER_H
#define AXIO_CBINARYWRITER_H

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
            mpStream->WriteS32(0); // Magic is written after the rest of the file has been successfully written
            SetVersion(skCurrentArchiveVersion, FileVersion, Game);
        }

        mpStream->WriteU16(mArchiveVersion);
        mpStream->WriteU16(mFileVersion);
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
            mpStream->WriteU32(mMagic);
            delete mpStream;
        }
    }

    bool IsValid() const { return mpStream->IsValid(); }

private:
    void InitParamStack()
    {
        mParamStack.reserve(20);
        mpStream->WriteU32(0xFFFFFFFF);
        mpStream->WriteU32(0); // Size filler
        mParamStack.push_back(SParameter{mpStream->Tell(), 0});
    }

public:
    // Interface
    bool ParamBegin(const char* pkName, uint32_t Flags) override
    {
        // Update parent param
        mParamStack.back().NumSubParams++;

        if (mParamStack.back().NumSubParams == 1)
            mpStream->WriteS32(-1); // Sub-param count filler

        // Write param metadata
        uint32_t ParamID = TString(pkName).Hash32();
        mpStream->WriteU32(ParamID);
        mpStream->WriteS32(-1); // Param size filler

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
        mpStream->WriteU32(ParamSize);

        // Write param child count
        if (rParam.NumSubParams > 0 || mParamStack.size() == 1)
        {
            mpStream->WriteU32(rParam.NumSubParams);
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
            mpStream->WriteS32(0);
    }

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

#endif // AXIO_CBINARYWRITER_H

