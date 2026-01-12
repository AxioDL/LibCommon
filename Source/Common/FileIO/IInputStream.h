#ifndef IINPUTSTREAM_H
#define IINPUTSTREAM_H

#include "Common/BasicTypes.h"
#include "Common/TString.h"

#include <bit>

class CFourCC;

class IInputStream
{
protected:
    std::endian mDataEndianness{};
    TString mDataSource;
    
public:
    bool ReadBool();
    int8_t ReadS8();
    uint8_t ReadU8();
    int16_t ReadS16();
    uint16_t ReadU16();
    int32_t ReadS32();
    uint32_t ReadU32();
    int64_t ReadS64();
    uint64_t ReadU64();
    float ReadF32();
    double ReadF64();
    CFourCC ReadFourCC();
    TString ReadString();
    TString ReadString(size_t Count);
    TString ReadSizedString();
    T16String Read16String();
    T16String Read16String(size_t Count);
    T16String ReadSized16String();

    int8_t PeekS8();
    uint8_t PeekU8();
    int16_t PeekS16();
    uint16_t PeekU16();
    int32_t PeekS32();
    uint32_t PeekU32();
    int64_t PeekS64();
    uint64_t PeekU64();
    float PeekF32();
    double PeekF64();
    CFourCC PeekFourCC();

    bool GoTo(uint32_t Address);
    bool Skip(int32_t SkipAmount);

    void SeekToBoundary(uint32_t Boundary);
    void SetEndianness(std::endian Endianness);
    void SetSourceString(const TString& rkSource);
    std::endian GetEndianness() const;
    TString GetSourceString() const;

    virtual ~IInputStream();
    virtual void ReadBytes(void *pDst, uint32_t Count) = 0;
    virtual bool Seek(int32_t Offset, uint32_t Origin) = 0;
    virtual bool Seek64(int64_t Offset, uint32_t Origin);
    virtual uint32_t Tell() const = 0;
    virtual uint64_t Tell64() const;
    virtual bool EoF() const = 0;
    virtual bool IsValid() const = 0;
    virtual uint32_t Size() const = 0;
};

#endif // IINPUTSTREAM_H
