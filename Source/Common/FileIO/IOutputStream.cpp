#include "IOutputStream.h"

IOutputStream::~IOutputStream() = default;

void IOutputStream::WriteBool(bool Val)
{
    const auto ChrVal = static_cast<char>(Val ? 1 : 0);
    WriteBytes(&ChrVal, 1);
}

void IOutputStream::WriteByte(int8_t Val)
{
    WriteBytes(&Val, 1);
}

void IOutputStream::WriteUByte(uint8_t Val)
{
    WriteByte(static_cast<int8_t>(Val));
}

void IOutputStream::WriteShort(int16_t Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);

    WriteBytes(&Val, 2);
}

void IOutputStream::WriteUShort(uint16_t Val)
{
    WriteShort(static_cast<int16_t>(Val));
}

void IOutputStream::WriteLong(int32_t Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);

    WriteBytes(&Val, 4);
}

void IOutputStream::WriteULong(uint32_t Val)
{
    WriteLong(static_cast<int32_t>(Val));
}

void IOutputStream::WriteLongLong(int64_t Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);

    WriteBytes(&Val, 8);
}

void IOutputStream::WriteULongLong(uint64_t Val)
{
    WriteLongLong(static_cast<int64_t>(Val));
}

void IOutputStream::WriteFloat(float Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<float>(std::byteswap(std::bit_cast<uint32_t>(Val)));

    WriteBytes(&Val, 4);
}

void IOutputStream::WriteDouble(double Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<double>(std::byteswap(std::bit_cast<uint64_t>(Val)));

    WriteBytes(&Val, 8);
}

void IOutputStream::WriteFourCC(uint32_t Val)
{
    if constexpr (std::endian::native == std::endian::little)
        Val = std::byteswap(Val);

    WriteBytes(&Val, 4);
}

void IOutputStream::WriteString(const TString& rkVal, int Count, bool Terminate)
{
    if (Count < 0)
        Count = rkVal.Size();

    WriteBytes(rkVal.Data(), Count);

    if (Terminate && (rkVal.IsEmpty() || rkVal[Count - 1] != 0))
        WriteByte(0);
}

void IOutputStream::WriteSizedString(const TString& rkVal)
{
    WriteLong(rkVal.Size());
    WriteBytes(rkVal.Data(), rkVal.Size());
}

void IOutputStream::Write16String(const T16String& rkVal, int Count, bool Terminate)
{
    if (Count < 0)
        Count = rkVal.Size();

    for (int ChrIdx = 0; ChrIdx < Count; ChrIdx++)
        WriteShort(rkVal[ChrIdx]);

    if (Terminate && (rkVal.IsEmpty() || rkVal[Count - 1] != 0))
        WriteShort(0);
}

void IOutputStream::WriteSized16String(const T16String& rkVal)
{
    WriteLong(rkVal.Size());

    for (size_t ChrIdx = 0; ChrIdx < rkVal.Size(); ChrIdx++)
        WriteShort(rkVal[ChrIdx]);
}

bool IOutputStream::GoTo(uint32_t Address)
{
    return Seek(Address, SEEK_SET);
}

bool IOutputStream::Skip(int32_t SkipAmount)
{
    return Seek(SkipAmount, SEEK_CUR);
}

void IOutputStream::WriteToBoundary(uint32_t Boundary, uint8_t Fill)
{
    const uint32_t Num = Boundary - (Tell() % Boundary);

    if (Num == Boundary)
        return;

    for (uint32_t iByte = 0; iByte < Num; iByte++)
        WriteByte(Fill);
}

void IOutputStream::SetEndianness(std::endian Endianness)
{
    mDataEndianness = Endianness;
}

void IOutputStream::SetDestString(const TString& rkDest)
{
    mDataDest = rkDest;
}

std::endian IOutputStream::GetEndianness() const
{
    return mDataEndianness;
}

TString IOutputStream::GetDestString() const
{
    return mDataDest;
}

bool IOutputStream::Seek64(int64_t Offset, uint32_t Origin)
{
    return Seek(static_cast<int32_t>(Offset), Origin);
}

uint64_t IOutputStream::Tell64() const
{
    return static_cast<uint64_t>(Tell());
}
