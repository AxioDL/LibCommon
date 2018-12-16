#ifndef IINPUTSTREAM_H
#define IINPUTSTREAM_H

#include "IOUtil.h"
#include "Common/BasicTypes.h"
#include "Common/TString.h"
#include <vector>

class IInputStream
{
protected:
    EEndian mDataEndianness;
    TString mDataSource;
    
public:
    bool ReadBool();
    int8 ReadByte();
    int16 ReadShort();
    int32 ReadLong();
    int64 ReadLongLong();
    float ReadFloat();
    double ReadDouble();
    uint32 ReadFourCC();
    TString ReadString();
    TString ReadString(uint32 Count);
    TString ReadSizedString();
    TWideString ReadWString();
    TWideString ReadWString(uint32 Count);
    TWideString ReadSizedWString();

    int8 PeekByte();
    int16 PeekShort();
    int32 PeekLong();
    int64 PeekLongLong();
    float PeekFloat();
    double PeekDouble();
    uint32 PeekFourCC();

    bool GoTo(uint32 Address);
    bool Skip(int32 SkipAmount);

    void SeekToBoundary(uint32 Boundary);
    void SetEndianness(EEndian Endianness);
    void SetSourceString(const TString& rkSource);
    EEndian GetEndianness() const;
    TString GetSourceString() const;

    virtual ~IInputStream();
    virtual void ReadBytes(void *pDst, uint32 Count) = 0;
    virtual bool Seek(int32 Offset, uint32 Origin) = 0;
    virtual bool Seek64(int64 Offset, uint32 Origin);
    virtual uint32 Tell() const = 0;
    virtual uint64 Tell64() const;
    virtual bool EoF() const = 0;
    virtual bool IsValid() const = 0;
    virtual uint32 Size() const = 0;
};

#endif // IINPUTSTREAM_H
