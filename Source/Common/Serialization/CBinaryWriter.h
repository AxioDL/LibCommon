#ifndef CBINARYWRITER
#define CBINARYWRITER

#include "IArchive.h"
#include "Common/CFourCC.h"

class CBinaryWriter : public IArchive
{
    struct SParameter
    {
        uint32 Offset;
        uint32 NumSubParams;
    };
    std::vector<SParameter> mParamStack;

    IOutputStream *mpStream = nullptr;
    uint32 mMagic = 0;
    bool mOwnsStream = false;

public:
    CBinaryWriter(const TString& rkFilename, uint32 Magic, uint16 FileVersion = 0, EGame Game = EGame::Invalid)
        : IArchive()
        , mMagic(Magic)
        , mOwnsStream(true)
    {
        mArchiveFlags = AF_Writer | AF_Binary;
        mpStream = new CFileOutStream(rkFilename, EEndian::BigEndian);

        if (mpStream->IsValid())
        {
            mpStream->WriteLong(0); // Magic is written after the rest of the file has been successfully written
            SetVersion(skCurrentArchiveVersion, FileVersion, Game);
        }

        mpStream->WriteShort(mArchiveVersion);
        mpStream->WriteShort(mFileVersion);
        mpStream->WriteFourCC( GameTo4CC(mGame).ToLong() );

        InitParamStack();
    }

    CBinaryWriter(IOutputStream *pStream, uint16 FileVersion = 0, EGame Game = EGame::Invalid)
        : IArchive()
    {
        ASSERT(pStream && pStream->IsValid());
        mArchiveFlags = AF_Writer | AF_Binary;
        mpStream = pStream;
        SetVersion(skCurrentArchiveVersion, FileVersion, Game);
        InitParamStack();
    }

    CBinaryWriter(IOutputStream *pStream, const CSerialVersion& rkVersion)
        : IArchive()
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
        mParamStack.push_back( SParameter { mpStream->Tell(), 0 } );
    }

public:
    // Interface
    bool ParamBegin(const char* pkName, uint32 Flags) override
    {
        // Update parent param
        mParamStack.back().NumSubParams++;

        if (mParamStack.back().NumSubParams == 1)
            mpStream->WriteLong(-1); // Sub-param count filler

        // Write param metadata
        uint32 ParamID = TString(pkName).Hash32();
        mpStream->WriteLong(ParamID);
        mpStream->WriteLong(-1); // Param size filler

        // Add new param to the stack
        mParamStack.push_back( SParameter { mpStream->Tell(), 0 } );

        return true;
    }

    void ParamEnd() override
    {
        // Write param size
        SParameter& rParam = mParamStack.back();
        uint32 StartOffset = rParam.Offset;
        uint32 EndOffset = mpStream->Tell();
        uint32 ParamSize = (EndOffset - StartOffset);

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

    bool PreSerializePointer(void*& Pointer, uint32 Flags) override
    {
        bool ValidPtr = (Pointer != nullptr);
        *this << SerialParameter("PointerValid", ValidPtr);
        return ValidPtr;
    }

    virtual void SerializeContainerSize(uint32& rSize, const TString& /*rkElemName*/)
    {
        // Normally handled by ParamBegin and ParamEnd but we need to do something here to account for zero-sized containers
        if (rSize == 0)
            mpStream->WriteLong(0);
    }

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

#endif // CBINARYWRITER

