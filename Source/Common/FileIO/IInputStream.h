#ifndef IINPUTSTREAM_H
#define IINPUTSTREAM_H

#include "Common/BasicTypes.h"
#include "Common/TString.h"

#include <bit>

class IInputStream
{
protected:
    std::endian mDataEndianness{};
    TString mDataSource;
    
public:
    bool ReadBool();
    int8_t ReadByte();
    uint8_t ReadUByte();
    int16_t ReadShort();
    uint16_t ReadUShort();
    int32_t ReadLong();
    uint32_t ReadULong();
    int64_t ReadLongLong();
    uint64_t ReadULongLong();
    float ReadFloat();
    double ReadDouble();
    uint32_t ReadFourCC();
    TString ReadString();
    TString ReadString(size_t Count);
    TString ReadSizedString();
    T16String Read16String();
    T16String Read16String(size_t Count);
    T16String ReadSized16String();

    int8_t PeekByte();
    uint8_t PeekUByte();
    int16_t PeekShort();
    uint16_t PeekUShort();
    int32_t PeekLong();
    uint32_t PeekULong();
    int64_t PeekLongLong();
    uint64_t PeekULongLong();
    float PeekFloat();
    double PeekDouble();
    uint32_t PeekFourCC();

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
