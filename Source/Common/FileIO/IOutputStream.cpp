#include "IOutputStream.h"

IOutputStream::~IOutputStream()
{
}

void IOutputStream::WriteBool(bool Val)
{
    char ChrVal = (Val ? 1 : 0);
    WriteBytes(&ChrVal, 1);
}

void IOutputStream::WriteByte(int8 Val)
{
    WriteBytes(&Val, 1);
}

void IOutputStream::WriteShort(int16 Val)
{
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    WriteBytes(&Val, 2);
}

void IOutputStream::WriteLong(int32 Val)
{
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    WriteBytes(&Val, 4);
}

void IOutputStream::WriteLongLong(int64 Val)
{
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    WriteBytes(&Val, 8);
}

void IOutputStream::WriteFloat(float Val)
{
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    WriteBytes(&Val, 4);
}

void IOutputStream::WriteDouble(double Val)
{
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    WriteBytes(&Val, 8);
}

void IOutputStream::WriteFourCC(uint32 Val)
{
    if (EEndian::SystemEndian == EEndian::LittleEndian) SwapBytes(Val);
    WriteBytes(&Val, 4);
}

void IOutputStream::WriteString(const TString& rkVal, int Count /*= -1*/, bool Terminate /*= true*/)
{
    if (Count < 0)
        Count = rkVal.Size();

    WriteBytes(rkVal.Data(), Count);

    if (Terminate && (rkVal.IsEmpty() || rkVal[Count-1] != 0))
        WriteByte(0);
}

void IOutputStream::WriteSizedString(const TString& rkVal)
{
    WriteLong(rkVal.Size());
    WriteBytes(rkVal.Data(), rkVal.Size());
}

void IOutputStream::Write16String(const T16String& rkVal, int Count /*= -1*/, bool Terminate /*= true*/)
{
    if (Count < 0)
        Count = rkVal.Size();

    for (int ChrIdx = 0; ChrIdx < Count; ChrIdx++)
        WriteShort(rkVal[ChrIdx]);

    if (Terminate && (rkVal.IsEmpty() || rkVal[Count-1] != 0))
        WriteShort(0);
}

void IOutputStream::WriteSized16String(const T16String& rkVal)
{
    WriteLong(rkVal.Size());

    for (uint32 ChrIdx = 0; ChrIdx < rkVal.Size(); ChrIdx++)
        WriteShort(rkVal[ChrIdx]);
}

bool IOutputStream::GoTo(uint32 Address)
{
    return Seek(Address, SEEK_SET);
}

bool IOutputStream::Skip(int32 SkipAmount)
{
    return Seek(SkipAmount, SEEK_CUR);
}

void IOutputStream::WriteToBoundary(uint32 Boundary, uint8 Fill)
{
    uint32 Num = Boundary - (Tell() % Boundary);
    if (Num == Boundary) return;
    for (uint32 iByte = 0; iByte < Num; iByte++)
        WriteByte(Fill);
}

void IOutputStream::SetEndianness(EEndian Endianness)
{
    mDataEndianness = Endianness;
}

void IOutputStream::SetDestString(const TString& rkDest)
{
    mDataDest = rkDest;
}

EEndian IOutputStream::GetEndianness() const
{
    return mDataEndianness;
}

TString IOutputStream::GetDestString() const
{
    return mDataDest;
}

bool IOutputStream::Seek64(int64 Offset, uint32 Origin)
{
    return Seek((int32) Offset, Origin);
}

uint64 IOutputStream::Tell64() const
{
    return (uint64) (Tell());
}
