#include "TString.h"

/** Decode functions */
static bool IsValidCodePoint(uint32 CodePoint)
{
    // 0xD800 to 0xDFFF are invalid code points; they are reserved for UTF-16 surrogate encoding
    // 0x10FFFF is the largest defined code point
    if ( (CodePoint >= 0xD800 && CodePoint <= 0xDFFF) || (CodePoint > 0x10FFFF) )
    {
        errorf("Invalid code point: 0x%X", CodePoint);
        return false;
    }
    else
    {
        return true;
    }
}

static uint32 DecodeCodePoint(const char*& pkInString)
{
    uint32 CodePoint;

    // One byte
    if ((pkInString[0] & 0x80) == 0)
    {
        CodePoint = (uint32) pkInString[0];
        pkInString++;
    }

    // Two bytes
    else if ((pkInString[0] & 0xE0) == 0xC0)
    {
        CodePoint = ( ((pkInString[0] & 0x1F) << 6) |
                      ((pkInString[1] & 0x3F) << 0) );
        pkInString += 2;
    }

    // Three bytes
    else if ((pkInString[0] & 0xF0) == 0xE0)
    {
        CodePoint = ( ((pkInString[0] & 0xF)  << 12) |
                      ((pkInString[1] & 0x3F) <<  6) |
                      ((pkInString[2] & 0x3F) <<  0) );
        pkInString += 3;
    }

    // Four bytes
    else if ((pkInString[0] & 0xF8) == 0xF0)
    {
        CodePoint = ( ((pkInString[0] & 0x7)  << 18) |
                      ((pkInString[1] & 0x3F) << 12) |
                      ((pkInString[2] & 0x3F) <<  6) |
                      ((pkInString[3] & 0x3F) <<  0) );
        pkInString += 4;
    }

    // The 5/6-byte variants are not technically needed, as Unicode code points are limited to 21 bits
    // However they are here anyway for the sake of completeness

    // Five bytes
    else if ((pkInString[0] & 0xFC) == 0xF8)
    {
        CodePoint = ( ((pkInString[0] & 0x3)  << 24) |
                      ((pkInString[1] & 0x3F) << 18) |
                      ((pkInString[2] & 0x3F) << 12) |
                      ((pkInString[3] & 0x3F) <<  6) |
                      ((pkInString[4] & 0x3F) <<  0) );
        pkInString += 5;
    }

    // Six bytes
    else if ((pkInString[0] & 0xFE) == 0xFC)
    {
        CodePoint = ( ((pkInString[0] & 0x1)  << 30) |
                      ((pkInString[1] & 0x3F) << 24) |
                      ((pkInString[2] & 0x3F) << 18) |
                      ((pkInString[3] & 0x3F) << 12) |
                      ((pkInString[4] & 0x3F) <<  6) |
                      ((pkInString[5] & 0x3F) <<  0) );
        pkInString += 6;
    }

    // Invalid?
    else
    {
        errorf("DecodeCodePoint() encountered invalid UTF-8 data: 0x%02X", pkInString[0]);
        return 0xFFFFFFFF;
    }

    return CodePoint;
}

static uint32 DecodeCodePoint(const char16_t*& pkInString)
{
    uint32 CodePoint;

    // Two bytes
    if (pkInString[0] <= 0xD7FF || pkInString[0] >= 0xE000)
    {
        CodePoint = (uint32) pkInString[0];
        pkInString++;
    }
    // Four bytes
    else
    {
        CodePoint = ((pkInString[0] - 0xD800) << 10) |
                    ((pkInString[1] - 0xDC00) <<  0) ;
        CodePoint += 0x10000;
        pkInString += 2;
    }

    return CodePoint;
}

static uint32 DecodeCodePoint(const char32_t*& pkInString)
{
    return *pkInString++;
}

/** Encode functions */
void TString::AppendCodePoint(uint32 CodePoint)
{
    ASSERT( IsValidCodePoint(CodePoint) );

    // One byte
    if (CodePoint <= 0x7F)
    {
        Append( (char) CodePoint );
    }
    // Two bytes
    else if (CodePoint <= 0x7FF)
    {
        Append( (char) (0xC0 | ((CodePoint >> 6) & 0x1F)) );
        Append( (char) (0x80 | ((CodePoint >> 0) & 0x3F)) );
    }
    // Three bytes
    else if (CodePoint <= 0xFFFF)
    {
        Append( (char) (0xE0 | ((CodePoint >> 12) & 0x0F)) );
        Append( (char) (0x80 | ((CodePoint >>  6) & 0x3F)) );
        Append( (char) (0x80 | ((CodePoint >>  0) & 0x3F)) );
    }
    // Four bytes
    else if (CodePoint <= 0x10FFFF)
    {
        Append( (char) (0xF0 | ((CodePoint >> 18) & 0x07)) );
        Append( (char) (0x80 | ((CodePoint >> 12) & 0x3F)) );
        Append( (char) (0x80 | ((CodePoint >>  6) & 0x3F)) );
        Append( (char) (0x80 | ((CodePoint >>  0) & 0x3F)) );
    }
}

void T16String::AppendCodePoint(uint32 CodePoint)
{
    ASSERT( IsValidCodePoint(CodePoint) );

    // Two bytes
    if (CodePoint < 0x10000)
    {
        Append( (char16_t) CodePoint );
    }
    // Four bytes
    else
    {
        CodePoint -= 0x10000;
        Append( (char16_t) (0xD800 | ((CodePoint >> 10) & 0x3FF)) );
        Append( (char16_t) (0xDC00 | ((CodePoint >>  0) & 0x3FF)) );
    }
}

void T32String::AppendCodePoint(uint32 CodePoint)
{
    ASSERT( IsValidCodePoint(CodePoint) );
    Append( (char32_t) CodePoint );
}

/** Conversion */
template<typename InStringType, typename OutStringType>
static OutStringType ConvertString(const InStringType& kInString)
{
    OutStringType Out;
    Out.Reserve( kInString.Size() );

    const typename InStringType::CharType* pkString = *kInString;

    while (*pkString != 0)
    {
        uint32 CodePoint = DecodeCodePoint(pkString);
        Out.AppendCodePoint(CodePoint);
    }

    Out.Shrink();
    return Out;
}

T16String TString::ToUTF16() const
{
    return ConvertString<TString, T16String>(*this);
}

T32String TString::ToUTF32() const
{
    return ConvertString<TString, T32String>(*this);
}

TString T16String::ToUTF8() const
{
    return ConvertString<T16String, TString>(*this);
}

T32String T16String::ToUTF32() const
{
    return ConvertString<T16String, T32String>(*this);
}

TString T32String::ToUTF8() const
{
    return ConvertString<T32String, TString>(*this);
}

T16String T32String::ToUTF16() const
{
    return ConvertString<T32String, T16String>(*this);
}
