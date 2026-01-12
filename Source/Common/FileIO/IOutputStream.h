#ifndef IOUTPUTSTREAM_H
#define IOUTPUTSTREAM_H

#include "Common/TString.h"

#include <bit>

class CFourCC;

class IOutputStream
{
protected:
    std::endian mDataEndianness{};
    TString mDataDest;

public:
    void WriteBool(bool Val);
    void WriteS8(int8_t Val);
    void WriteU8(uint8_t Val);
    void WriteS16(int16_t Val);
    void WriteU16(uint16_t Val);
    void WriteS32(int32_t Val);
    void WriteU32(uint32_t Val);
    void WriteS64(int64_t Val);
    void WriteU64(uint64_t Val);
    void WriteF32(float Val);
    void WriteF64(double Val);
    void WriteFourCC(const CFourCC& fcc);
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
