#include "IOutputStream.h"

#include "Common/CFourCC.h"

IOutputStream::~IOutputStream() = default;

void IOutputStream::WriteBool(bool Val)
{
    const auto ByteVal = static_cast<int8_t>(Val ? 1 : 0);
    WriteS8(ByteVal);
}

void IOutputStream::WriteS8(int8_t Val)
{
    WriteBytes(&Val, sizeof(Val));
}

void IOutputStream::WriteU8(uint8_t Val)
{
    WriteS8(static_cast<int8_t>(Val));
}

void IOutputStream::WriteS16(int16_t Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);

    WriteBytes(&Val, sizeof(Val));
}

void IOutputStream::WriteU16(uint16_t Val)
{
    WriteS16(static_cast<int16_t>(Val));
}

void IOutputStream::WriteS32(int32_t Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);

    WriteBytes(&Val, sizeof(Val));
}

void IOutputStream::WriteU32(uint32_t Val)
{
    WriteS32(static_cast<int32_t>(Val));
}

void IOutputStream::WriteS64(int64_t Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::byteswap(Val);

    WriteBytes(&Val, sizeof(Val));
}

void IOutputStream::WriteU64(uint64_t Val)
{
    WriteS64(static_cast<int64_t>(Val));
}

void IOutputStream::WriteF32(float Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<float>(std::byteswap(std::bit_cast<uint32_t>(Val)));

    WriteBytes(&Val, sizeof(Val));
}

void IOutputStream::WriteF64(double Val)
{
    if (mDataEndianness != std::endian::native)
        Val = std::bit_cast<double>(std::byteswap(std::bit_cast<uint64_t>(Val)));

    WriteBytes(&Val, sizeof(Val));
}

void IOutputStream::WriteFourCC(const CFourCC& fcc)
{
    auto val = fcc.ToU32();

    if constexpr (std::endian::native == std::endian::little)
        val = std::byteswap(val);

    WriteBytes(&val, sizeof(val));
}

void IOutputStream::WriteString(const TString& rkVal, int Count, bool Terminate)
{
    if (Count < 0)
        Count = rkVal.Size();

    WriteBytes(rkVal.Data(), Count);

    if (Terminate && (rkVal.IsEmpty() || rkVal[Count - 1] != 0))
        WriteS8(0);
}

void IOutputStream::WriteSizedString(const TString& rkVal)
{
    WriteU32(rkVal.Size());
    WriteBytes(rkVal.Data(), rkVal.Size());
}

void IOutputStream::Write16String(const T16String& rkVal, int Count, bool Terminate)
{
    if (Count < 0)
        Count = rkVal.Size();

    for (int ChrIdx = 0; ChrIdx < Count; ChrIdx++)
        WriteS16(rkVal[ChrIdx]);

    if (Terminate && (rkVal.IsEmpty() || rkVal[Count - 1] != 0))
        WriteS16(0);
}

void IOutputStream::WriteSized16String(const T16String& rkVal)
{
    WriteU32(rkVal.Size());

    for (size_t ChrIdx = 0; ChrIdx < rkVal.Size(); ChrIdx++)
        WriteS16(rkVal[ChrIdx]);
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
    const auto Num = Boundary - (Tell() % Boundary);

    if (Num == Boundary)
        return;

    for (uint32_t iByte = 0; iByte < Num; iByte++)
        WriteU8(Fill);
}

void IOutputStream::SetEndianness(std::endian Endianness)
{
    mDataEndianness = Endianness;
}

std::endian IOutputStream::GetEndianness() const
{
    return mDataEndianness;
}

bool IOutputStream::Seek64(int64_t Offset, uint32_t Origin)
{
    return Seek(static_cast<int32_t>(Offset), Origin);
}

uint64_t IOutputStream::Tell64() const
{
    return static_cast<uint64_t>(Tell());
}
