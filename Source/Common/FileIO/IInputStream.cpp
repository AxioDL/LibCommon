#include "IInputStream.h"

#include "Common/CFourCC.h"
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

int8_t IInputStream::ReadS8()
{
    int8_t Val;
    ReadBytes(&Val, sizeof(Val));
    return Val;
}

uint8_t IInputStream::ReadU8()
{
    return static_cast<uint8_t>(ReadS8());
}

int16_t IInputStream::ReadS16()
{
    int16_t Val;
    ReadBytes(&Val, sizeof(Val));
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);
    return Val;
}

uint16_t IInputStream::ReadU16()
{
    return static_cast<uint16_t>(ReadS16());
}

int32_t IInputStream::ReadS32()
{
    int32_t Val;
    ReadBytes(&Val, sizeof(Val));
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);
    return Val;
}

uint32_t IInputStream::ReadU32()
{
    return static_cast<uint32_t>(ReadS32());
}

int64_t IInputStream::ReadS64()
{
    int64_t Val;
    ReadBytes(&Val, sizeof(Val));
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);
    return Val;
}

uint64_t IInputStream::ReadU64()
{
    return static_cast<uint64_t>(ReadS64());
}

float IInputStream::ReadF32()
{
    float Val;
    ReadBytes(&Val, sizeof(Val));
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<float>(std::byteswap(std::bit_cast<uint32_t>(Val)));
    return Val;
}

double IInputStream::ReadF64()
{
    double Val;
    ReadBytes(&Val, sizeof(Val));
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<double>(std::byteswap(std::bit_cast<uint64_t>(Val)));
    return Val;
}

CFourCC IInputStream::ReadFourCC()
{
    uint32_t Val;
    ReadBytes(&Val, sizeof(Val));
    if constexpr (std::endian::native == std::endian::little)
        Val = std::byteswap(Val);
    return CFourCC(Val);
}

TString IInputStream::ReadString()
{
    TString Str;
    char Chr;

    do
    {
        Chr = ReadS8();
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
    const auto StringSize = ReadU32();
    return ReadString(StringSize);
}

T16String IInputStream::Read16String()
{
    T16String Out;
    char16_t Chr = 1;

    do
    {
        Chr = ReadS16();
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
        Out[i] = ReadS16();
    }

    return Out;
}

T16String IInputStream::ReadSized16String()
{
    const auto StringSize = ReadU32();
    return Read16String(StringSize);
}

int8_t IInputStream::PeekS8()
{
    const auto Val = ReadS8();
    Seek(-1, SEEK_CUR);
    return Val;
}

uint8_t IInputStream::PeekU8()
{
    return static_cast<uint8_t>(PeekS8());
}

int16_t IInputStream::PeekS16()
{
    const auto Val = ReadS16();
    Seek(-2, SEEK_CUR);
    return Val;
}

uint16_t IInputStream::PeekU16()
{
    return static_cast<uint16_t>(PeekS16());
}

int32_t IInputStream::PeekS32()
{
    const auto Val = ReadS32();
    Seek(-4, SEEK_CUR);
    return Val;
}

uint32_t IInputStream::PeekU32()
{
    return static_cast<uint32_t>(PeekS32());
}

int64_t IInputStream::PeekS64()
{
    const auto Val = ReadS64();
    Seek(-8, SEEK_CUR);
    return Val;
}

uint64_t IInputStream::PeekU64()
{
    return static_cast<uint64_t>(PeekS64());
}

float IInputStream::PeekF32()
{
    const auto Val = ReadF32();
    Seek(-4, SEEK_CUR);
    return Val;
}

double IInputStream::PeekF64()
{
    const auto Val = ReadF64();
    Seek(-8, SEEK_CUR);
    return Val;
}

CFourCC IInputStream::PeekFourCC()
{
    const auto Val = ReadFourCC();
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
