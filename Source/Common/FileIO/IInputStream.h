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
    uint8 ReadUByte();
    int16 ReadShort();
    uint16 ReadUShort();
    int32 ReadLong();
    uint32 ReadULong();
    int64 ReadLongLong();
    uint64 ReadULongLong();
    float ReadFloat();
    double ReadDouble();
    uint32 ReadFourCC();
    TString ReadString();
    TString ReadString(size_t Count);
    TString ReadSizedString();
    T16String Read16String();
    T16String Read16String(size_t Count);
    T16String ReadSized16String();

    int8 PeekByte();
    uint8 PeekUByte();
    int16 PeekShort();
    uint16 PeekUShort();
    int32 PeekLong();
    uint32 PeekULong();
    int64 PeekLongLong();
    uint64 PeekULongLong();
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
