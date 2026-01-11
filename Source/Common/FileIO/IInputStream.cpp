#include "IInputStream.h"
#include "Common/Log.h"

#include <bit>
#include <cstdio>

IInputStream::~IInputStream() = default;

bool IInputStream::ReadBool()
{
    char Val;
    ReadBytes(&Val, 1);

#if _DEBUG
    if (Val != 0 && Val != 1)
    {
        NLog::Error("ReadBool() got invalid value: {}", Val);
    }
#endif

    return Val != 0;
}

int8_t IInputStream::ReadByte()
{
    int8_t Val;
    ReadBytes(&Val, 1);
    return Val;
}

uint8_t IInputStream::ReadUByte()
{
    return static_cast<uint8_t>(ReadByte());
}

int16_t IInputStream::ReadShort()
{
    int16_t Val;
    ReadBytes(&Val, 2);
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);
    return Val;
}

uint16_t IInputStream::ReadUShort()
{
    return static_cast<uint16_t>(ReadShort());
}

int32_t IInputStream::ReadLong()
{
    int32_t Val;
    ReadBytes(&Val, 4);
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);
    return Val;
}

uint32_t IInputStream::ReadULong()
{
    return static_cast<uint32_t>(ReadLong());
}

int64_t IInputStream::ReadLongLong()
{
    int64_t Val;
    ReadBytes(&Val, 8);
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);
    return Val;
}

uint64_t IInputStream::ReadULongLong()
{
    return static_cast<uint64_t>(ReadLongLong());
}

float IInputStream::ReadFloat()
{
    float Val;
    ReadBytes(&Val, 4);
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<float>(std::byteswap(std::bit_cast<uint32_t>(Val)));
    return Val;
}

double IInputStream::ReadDouble()
{
    double Val;
    ReadBytes(&Val, 8);
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<double>(std::byteswap(std::bit_cast<uint64_t>(Val)));
    return Val;
}

uint32_t IInputStream::ReadFourCC()
{
    uint32_t Val;
    ReadBytes(&Val, 4);
    if constexpr (std::endian::native == std::endian::little)
        Val = std::byteswap(Val);
    return Val;
}

TString IInputStream::ReadString()
{
    TString Str;
    char Chr;

    do
    {
        Chr = ReadByte();
        if (Chr != 0)
            Str.Append(Chr);
    }
    while (Chr != 0 && !EoF());

    return Str;
}

TString IInputStream::ReadString(size_t Count)
{
    TString Str(Count, 0);
    ReadBytes(Str.Data(), Count);
    return Str;
}

TString IInputStream::ReadSizedString()
{
    const uint32 StringSize = ReadULong();
    return ReadString(StringSize);
}

T16String IInputStream::Read16String()
{
    T16String Out;
    char16_t Chr = 1;

    do
    {
        Chr = ReadShort();
        if (Chr != 0)
            Out.Append(Chr);
    }
    while (Chr != 0 && !EoF());

    return Out;
}

T16String IInputStream::Read16String(size_t Count)
{
    T16String Out(Count, 0);

    for (size_t i = 0; i < Count; i++)
    {
        Out[i] = ReadShort();
    }

    return Out;
}

T16String IInputStream::ReadSized16String()
{
    const uint32_t StringSize = ReadULong();
    return Read16String(StringSize);
}

int8_t IInputStream::PeekByte()
{
    const int8_t Val = ReadByte();
    Seek(-1, SEEK_CUR);
    return Val;
}

uint8_t IInputStream::PeekUByte()
{
    return static_cast<uint8_t>(PeekByte());
}

int16_t IInputStream::PeekShort()
{
    const int16_t Val = ReadShort();
    Seek(-2, SEEK_CUR);
    return Val;
}

uint16_t IInputStream::PeekUShort()
{
    return static_cast<uint16_t>(PeekShort());
}

int32_t IInputStream::PeekLong()
{
    const int32_t Val = ReadLong();
    Seek(-4, SEEK_CUR);
    return Val;
}

uint32_t IInputStream::PeekULong()
{
    return static_cast<uint32_t>(PeekLong());
}

int64_t IInputStream::PeekLongLong()
{
    const int64_t Val = ReadLongLong();
    Seek(-8, SEEK_CUR);
    return Val;
}

uint64_t IInputStream::PeekULongLong()
{
    return static_cast<uint64_t>(PeekLongLong());
}

float IInputStream::PeekFloat()
{
    const float Val = ReadFloat();
    Seek(-4, SEEK_CUR);
    return Val;
}

double IInputStream::PeekDouble()
{
    const double Val = ReadDouble();
    Seek(-8, SEEK_CUR);
    return Val;
}

uint32_t IInputStream::PeekFourCC()
{
    const uint32_t Val = ReadFourCC();
    Seek(-4, SEEK_CUR);
    return Val;
}

bool IInputStream::GoTo(uint32_t Address)
{
    return Seek(Address, SEEK_SET);
}

bool IInputStream::Skip(int32_t SkipAmount)
{
    return Seek(SkipAmount, SEEK_CUR);
}

void IInputStream::SeekToBoundary(uint32_t Boundary)
{
    const uint32_t Num = Boundary - (Tell() % Boundary);

    if (Num == Boundary)
        return;

    Seek(Num, SEEK_CUR);
}

void IInputStream::SetEndianness(std::endian Endianness)
{
    mDataEndianness = Endianness;
}

void IInputStream::SetSourceString(const TString& rkSource)
{
    mDataSource = rkSource;
}

std::endian IInputStream::GetEndianness() const
{
    return mDataEndianness;
}

TString IInputStream::GetSourceString() const
{
    return mDataSource;
}

bool IInputStream::Seek64(int64_t Offset, uint32_t Origin)
{
    return Seek(static_cast<int32_t>(Offset), Origin);
}

uint64_t IInputStream::Tell64() const
{
    return static_cast<uint64_t>(Tell());
}
