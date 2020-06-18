#ifndef IOUTPUTSTREAM_H
#define IOUTPUTSTREAM_H

#include "IOUtil.h"
#include "Common/TString.h"

class IOutputStream
{
protected:
    EEndian mDataEndianness;
    TString mDataDest;

public:
    void WriteBool(bool Val);
    void WriteByte(int8 Val);
    void WriteUByte(uint8 Val);
    void WriteShort(int16 Val);
    void WriteUShort(uint16 Val);
    void WriteLong(int32 Val);
    void WriteULong(uint32 Val);
    void WriteLongLong(int64 Val);
    void WriteULongLong(uint64 Val);
    void WriteFloat(float Val);
    void WriteDouble(double Val);
    void WriteFourCC(uint32 Val);
    void WriteString(const TString& rkVal, int Count = -1, bool Terminate = true);
    void WriteSizedString(const TString& rkVal);
    void Write16String(const T16String& rkVal, int Count = -1, bool Terminate = true);
    void WriteSized16String(const T16String& rkVal);

    bool GoTo(uint32 Address);
    bool Skip(int32 SkipAmount);

    void WriteToBoundary(uint32 Boundary, uint8 Fill);
    void SetEndianness(EEndian Endianness);
    void SetDestString(const TString& rkDest);
    EEndian GetEndianness() const;
    TString GetDestString() const;

    virtual ~IOutputStream();
    virtual void WriteBytes(const void *pkSrc, uint32 Count) = 0;
    virtual bool Seek(int32 Offset, uint32 Origin) = 0;
    virtual bool Seek64(int64 Offset, uint32 Origin);
    virtual uint32 Tell() const = 0;
    virtual uint64 Tell64() const;
    virtual bool EoF() const = 0;
    virtual bool IsValid() const = 0;
    virtual uint32 Size() const = 0;
};
#endif // COUTPUTSTREAM_H
