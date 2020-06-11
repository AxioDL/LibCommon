#ifndef CBINARYREADER
#define CBINARYREADER

#include "IArchive.h"
#include "CSerialVersion.h"
#include "Common/CFourCC.h"

class CBinaryReader : public IArchive
{
    struct SBinaryParm
    {
        uint32 Offset;
        uint32 Size;
        uint32 NumChildren;
        uint32 ChildIndex;
    };
    std::vector<SBinaryParm> mBinaryParmStack;

    IInputStream *mpStream = nullptr;
    bool mMagicValid = false;
    bool mOwnsStream = false;
    bool mInAttribute = false;

public:
    CBinaryReader(const TString& rkFilename, uint32 Magic)
        : IArchive()
        , mOwnsStream(true)
    {
        mArchiveFlags = AF_Reader | AF_Binary;
        mpStream = new CFileInStream(rkFilename, EEndian::BigEndian);

        if (mpStream->IsValid())
        {
            mMagicValid = (mpStream->ReadLong() == Magic);
        }

        // check for incorrectly-formatted data (bug with older version of binary serializer)
        if (mpStream->PeekShort() == -1)
        {
            InitParamStack();
            SerializeVersion();
        }
        else
        {
            mArchiveVersion = mpStream->ReadShort();
            mFileVersion = mpStream->ReadShort();
            mGame = GameFrom4CC( mpStream->ReadFourCC() );
            InitParamStack();
        }
    }

    CBinaryReader(IInputStream *pStream, const CSerialVersion& rkVersion)
        : IArchive()
        , mMagicValid(true)
    {
        ASSERT(pStream && pStream->IsValid());
        mArchiveFlags = AF_Reader | AF_Binary;
        mpStream = pStream;
        SetVersion(rkVersion);

        InitParamStack();
    }

    ~CBinaryReader() override
    {
        if (mOwnsStream) delete mpStream;
    }

    bool IsValid() const { return mpStream->IsValid() && mMagicValid; }

private:
    void InitParamStack()
    {
        mpStream->Skip(4); // Skip root ID (which is always -1)
        uint32 Size = ReadSize();
        uint32 Offset = mpStream->Tell();
        uint32 NumChildren = ReadSize();
        mBinaryParmStack.push_back( SBinaryParm { Offset, Size, NumChildren, 0 } );
        mBinaryParmStack.reserve(20);
    }

public:
    // Interface
    uint32 ReadSize()
    {
        return (mArchiveVersion < eArVer_32BitBinarySize ? (uint32) mpStream->ReadShort() : mpStream->ReadLong());
    }

    bool ParamBegin(const char* pkName, uint32 Flags) override
    {
        // If this is the parent parameter's first child, then read the child count
        if (mBinaryParmStack.back().NumChildren == 0xFFFFFFFF)
        {
            mBinaryParmStack.back().NumChildren = ReadSize();
        }

        // Save current offset
        uint32 Offset = mpStream->Tell();
        uint32 ParamID = TString(pkName).Hash32();

        // Check the next parameter ID first and check whether it's a match for the current parameter
        if (mBinaryParmStack.back().ChildIndex < mBinaryParmStack.back().NumChildren)
        {
            uint32 NextID = mpStream->ReadLong();
            uint32 NextSize = ReadSize();

            // Does the next parameter ID match the current one?
            if (NextID == ParamID || (Flags & SH_IgnoreName))
            {
                mBinaryParmStack.push_back( SBinaryParm { mpStream->Tell(), NextSize, 0xFFFFFFFF, 0 } );
                return true;
            }
        }

        // It's not a match - return to the parent parameter's first child and check all children to find a match
        if (!mBinaryParmStack.empty())
        {
            uint32 ParentOffset = mBinaryParmStack.back().Offset;
            uint32 NumChildren = mBinaryParmStack.back().NumChildren;
            mpStream->GoTo(ParentOffset);

            for (uint32 ChildIdx = 0; ChildIdx < NumChildren; ChildIdx++)
            {
                uint32 ChildID = mpStream->ReadLong();
                uint32 ChildSize = ReadSize();

                if (ChildID != ParamID)
                    mpStream->Skip(ChildSize);
                else
                {
                    mBinaryParmStack.back().ChildIndex = ChildIdx;
                    mBinaryParmStack.push_back( SBinaryParm { mpStream->Tell(), ChildSize, 0xFFFFFFFF, 0 } );
                    return true;
                }
            }
        }

        // None of the children were a match - this parameter isn't in the file
        mpStream->GoTo(Offset);
        return false;
    }

    void ParamEnd() override
    {
        // Make sure we're at the end of the parameter
        SBinaryParm& rParam = mBinaryParmStack.back();
        uint32 EndOffset = rParam.Offset + rParam.Size;
        mpStream->GoTo(EndOffset);
        mBinaryParmStack.pop_back();

        // Increment parent child index
        if (!mBinaryParmStack.empty())
            mBinaryParmStack.back().ChildIndex++;
    }

    bool PreSerializePointer(void*& Pointer, uint32 Flags) override
    {
        if (ArchiveVersion() >= eArVer_Refactor)
        {
            bool ValidPtr = (Pointer != nullptr);
            *this << SerialParameter("PointerValid", ValidPtr);
            return ValidPtr;
        }
        else
        {
            return true;
        }
    }

    virtual void SerializeContainerSize(uint32& rSize, const TString& /*rkElemName*/)
    {
        // Mostly handled by ParamBegin, we just need to return the size correctly so the container can be resized
        rSize = (mArchiveVersion < eArVer_32BitBinarySize ? (uint32) mpStream->PeekShort() : mpStream->PeekLong());
    }

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
    void SerializePrimitive(CAssetID& rValue, uint32 Flags) override { rValue = CAssetID(*mpStream, Game()); }
    void SerializeBulkData(void* pData, uint32 Size, uint32 Flags) override { mpStream->ReadBytes(pData, Size); }
};

#endif // CBINARYREADER

