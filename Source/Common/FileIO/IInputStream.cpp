#include "IInputStream.h"
#include "Common/Macros.h"

IInputStream::~IInputStream()
{
}

bool IInputStream::ReadBool()
{
    char Val;
    ReadBytes(&Val, 1);

#if _DEBUG
    if (Val != 0 && Val != 1)
    {
        errorf("ReadBool() got invalid value: %d", Val);
    }
#endif

    return (Val != 0 ? true : false);
}

int8 IInputStream::ReadByte()
{
    int8 Val;
    ReadBytes(&Val, 1);
    return Val;
}

int16 IInputStream::ReadShort()
{
    int16 Val;
    ReadBytes(&Val, 2);
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    return Val;
}

int32 IInputStream::ReadLong()
{
    int32 Val;
    ReadBytes(&Val, 4);
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    return Val;
}

int64 IInputStream::ReadLongLong()
{
    int64 Val;
    ReadBytes(&Val, 8);
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    return Val;
}

float IInputStream::ReadFloat()
{
    float Val;
    ReadBytes(&Val, 4);
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    return Val;
}

double IInputStream::ReadDouble()
{
    double Val;
    ReadBytes(&Val, 8);
    if (mDataEndianness != EEndian::SystemEndian) SwapBytes(Val);
    return Val;
}

uint32 IInputStream::ReadFourCC()
{
    uint32 Val;
    ReadBytes(&Val, 4);
    if (EEndian::SystemEndian == EEndian::LittleEndian) SwapBytes(Val);
    return Val;
}

TString IInputStream::ReadString()
{
    TString Str;
    char Chr;

    do
    {
        Chr = ReadByte();
        if (Chr != 0) Str.Append(Chr);
    }
    while ((Chr != 0) && (!EoF()));

    return Str;
}

TString IInputStream::ReadString(uint32 Count)
{
    TString Str(Count, 0);
    ReadBytes(&Str[0], Count);
    return Str;
}

TString IInputStream::ReadSizedString()
{
    uint32 StringSize = ReadLong();
    return ReadString(StringSize);
}

T16String IInputStream::Read16String()
{
    T16String Out;
    char16_t Chr = 1;

    do
    {
        Chr = ReadShort();
        if (Chr != 0) Out.Append(Chr);
    }
    while (Chr != 0 && !EoF());

    return Out;
}

T16String IInputStream::Read16String(uint32 Count)
{
    T16String Out(Count, 0);

    for( uint32 i=0; i<Count; i++ )
    {
        Out[i] = ReadShort();
    }

    return Out;
}

T16String IInputStream::ReadSized16String()
{
    uint StringSize = ReadLong();
    return Read16String(StringSize);
}

int8 IInputStream::PeekByte()
{
    int8 Val = ReadByte();
    Seek(-1, SEEK_CUR);
    return Val;
}

int16 IInputStream::PeekShort()
{
    int16 Val = ReadShort();
    Seek(-2, SEEK_CUR);
    return Val;
}

int32 IInputStream::PeekLong()
{
    int32 Val = ReadLong();
    Seek(-4, SEEK_CUR);
    return Val;
}

int64 IInputStream::PeekLongLong()
{
    int64 Val = ReadLongLong();
    Seek(-8, SEEK_CUR);
    return Val;
}

float IInputStream::PeekFloat()
{
    float Val = ReadFloat();
    Seek(-4, SEEK_CUR);
    return Val;
}

double IInputStream::PeekDouble()
{
    double Val = ReadDouble();
    Seek(-8, SEEK_CUR);
    return Val;
}

uint32 IInputStream::PeekFourCC()
{
    long Val = ReadFourCC();
    Seek(-4, SEEK_CUR);
    return Val;
}

bool IInputStream::GoTo(uint32 Address)
{
    return Seek(Address, SEEK_SET);
}

bool IInputStream::Skip(int32 SkipAmount)
{
    return Seek(SkipAmount, SEEK_CUR);
}

void IInputStream::SeekToBoundary(uint32 Boundary)
{
    uint32 Num = Boundary - (Tell() % Boundary);
    if (Num == Boundary) return;
    else Seek(Num, SEEK_CUR);
}

void IInputStream::SetEndianness(EEndian Endianness)
{
    mDataEndianness = Endianness;
}

void IInputStream::SetSourceString(const TString& rkSource)
{
    mDataSource = rkSource;
}

EEndian IInputStream::GetEndianness() const
{
    return mDataEndianness;
}

TString IInputStream::GetSourceString() const
{
    return mDataSource;
}

bool IInputStream::Seek64(int64 Offset, uint32 Origin)
{
    return Seek((int32) Offset, Origin);
}

uint64 IInputStream::Tell64() const
{
    return (uint64) Tell();
}
