#ifndef IOUTPUTSTREAM_H
#define IOUTPUTSTREAM_H

#include "Common/TString.h"

#include <bit>

class IOutputStream
{
protected:
    std::endian mDataEndianness{};
    TString mDataDest;

public:
    void WriteBool(bool Val);
    void WriteByte(int8_t Val);
    void WriteUByte(uint8_t Val);
    void WriteShort(int16_t Val);
    void WriteUShort(uint16_t Val);
    void WriteLong(int32_t Val);
    void WriteULong(uint32_t Val);
    void WriteLongLong(int64_t Val);
    void WriteULongLong(uint64_t Val);
    void WriteFloat(float Val);
    void WriteDouble(double Val);
    void WriteFourCC(uint32_t Val);
    void WriteString(const TString& rkVal, int Count = -1, bool Terminate = true);
    void WriteSizedString(const TString& rkVal);
    void Write16String(const T16String& rkVal, int Count = -1, bool Terminate = true);
    void WriteSized16String(const T16String& rkVal);

    bool GoTo(uint32_t Address);
    bool Skip(int32_t SkipAmount);

    void WriteToBoundary(uint32_t Boundary, uint8_t Fill);
    void SetEndianness(std::endian Endianness);
    void SetDestString(const TString& rkDest);
    std::endian GetEndianness() const;
    TString GetDestString() const;

    virtual ~IOutputStream();
    virtual void WriteBytes(const void *pkSrc, uint32_t Count) = 0;
    virtual bool Seek(int32_t Offset, uint32_t Origin) = 0;
    virtual bool Seek64(int64_t Offset, uint32_t Origin);
    virtual uint32_t Tell() const = 0;
    virtual uint64_t Tell64() const;
    virtual bool EoF() const = 0;
    virtual bool IsValid() const = 0;
    virtual uint32_t Size() const = 0;
};
#endif // COUTPUTSTREAM_H
