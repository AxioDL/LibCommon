#ifndef CBINARYREADER
#define CBINARYREADER

#include "Common/CFourCC.h"
#include "Common/FileIO/CFileInStream.h"
#include "Common/Serialization/IArchive.h"
#include "Common/Serialization/CSerialVersion.h"

class CBinaryReader : public IArchive
{
    struct SBinaryParm
    {
        uint32_t Offset;
        uint32_t Size;
        uint32_t NumChildren;
        uint32_t ChildIndex;
    };
    std::vector<SBinaryParm> mBinaryParmStack;

    IInputStream *mpStream = nullptr;
    bool mMagicValid = false;
    bool mOwnsStream = false;
    bool mInAttribute = false;

public:
    explicit CBinaryReader(const TString& rkFilename, uint32_t Magic)
        : mOwnsStream(true)
    {
        mArchiveFlags = AF_Reader | AF_Binary;
        mpStream = new CFileInStream(rkFilename, std::endian::big);

        if (mpStream->IsValid())
        {
            mMagicValid = (mpStream->ReadU32() == Magic);
        }

        // check for incorrectly-formatted data (bug with older version of binary serializer)
        if (mpStream->PeekS16() == -1)
        {
            InitParamStack();
            SerializeVersion();
        }
        else
        {
            mArchiveVersion = mpStream->ReadU16();
            mFileVersion = mpStream->ReadU16();
            mGame = GameFrom4CC(mpStream->ReadFourCC());
            InitParamStack();
        }
    }

    explicit CBinaryReader(IInputStream *pStream, const CSerialVersion& rkVersion)
        : mMagicValid(true)
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
        const auto Size = ReadSize();
        const auto Offset = mpStream->Tell();
        const auto NumChildren = ReadSize();
        mBinaryParmStack.push_back(SBinaryParm{Offset, Size, NumChildren, 0});
        mBinaryParmStack.reserve(20);
    }

public:
    // Interface
    uint32_t ReadSize()
    {
        return (mArchiveVersion < eArVer_32BitBinarySize ? (uint32_t) mpStream->ReadU16() : mpStream->ReadU32());
    }

    bool ParamBegin(const char* pkName, uint32_t Flags) override
    {
        // If this is the parent parameter's first child, then read the child count
        if (mBinaryParmStack.back().NumChildren == 0xFFFFFFFF)
        {
            mBinaryParmStack.back().NumChildren = ReadSize();
        }

        // Save current offset
        const auto Offset = mpStream->Tell();
        const auto ParamID = TString(pkName).Hash32();

        // Check the next parameter ID first and check whether it's a match for the current parameter
        if (mBinaryParmStack.back().ChildIndex < mBinaryParmStack.back().NumChildren)
        {
            const auto NextID = mpStream->ReadU32();
            const auto NextSize = ReadSize();

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
            const auto ParentOffset = mBinaryParmStack.back().Offset;
            const auto NumChildren = mBinaryParmStack.back().NumChildren;
            mpStream->GoTo(ParentOffset);

            for (uint32_t ChildIdx = 0; ChildIdx < NumChildren; ChildIdx++)
            {
                const auto ChildID = mpStream->ReadU32();
                const auto ChildSize = ReadSize();

                if (ChildID != ParamID)
                {
                    mpStream->Skip(ChildSize);
                }
                else
                {
                    mBinaryParmStack.back().ChildIndex = ChildIdx;
                    mBinaryParmStack.push_back(SBinaryParm{mpStream->Tell(), ChildSize, 0xFFFFFFFF, 0});
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
        const auto& rParam = mBinaryParmStack.back();
        const auto EndOffset = rParam.Offset + rParam.Size;
        mpStream->GoTo(EndOffset);
        mBinaryParmStack.pop_back();

        // Increment parent child index
        if (!mBinaryParmStack.empty())
            mBinaryParmStack.back().ChildIndex++;
    }

    bool PreSerializePointer(void*& Pointer, uint32_t Flags) override
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

    virtual void SerializeContainerSize(uint32_t& rSize, const TString& /*rkElemName*/)
    {
        // Mostly handled by ParamBegin, we just need to return the size correctly so the container can be resized
        rSize = (mArchiveVersion < eArVer_32BitBinarySize ? (uint32_t) mpStream->PeekU16() : mpStream->PeekU32());
    }

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
    void SerializePrimitive(CAssetID& rValue, uint32_t Flags) override { rValue = CAssetID(*mpStream, Game()); }
    void SerializeBulkData(void* pData, uint32_t Size, uint32_t Flags) override { mpStream->ReadBytes(pData, Size); }
};

#endif // CBINARYREADER

