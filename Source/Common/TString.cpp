#include "TString.h"
#include "Hash/CCRC32.h"
#include "Hash/CFNV1A.h"
#include "FileIO/IOUtil.h"
#include <codecvt>
#include <locale>

// ************ TString ************
uint32 TString::Hash32() const
{
    CCRC32 Hash;
    Hash.Hash(**this);
    return Hash.Digest();
}

uint64 TString::Hash64() const
{
    // todo: replace with MD5
    CFNV1A Hash(CFNV1A::k64Bit);
    Hash.HashString(*this);
    return Hash.GetHash64();
}

TWideString TString::ToUTF16() const
{
    TWideString Out;
    Out.Reserve(Size());

    const char *pkCStr = CString();

    while (pkCStr[0])
    {
        // Step 1: decode UTF-8 code point
        uint32 CodePoint;

        // One byte
        if ((pkCStr[0] & 0x80) == 0)
        {
            CodePoint = (uint32) pkCStr[0];
            pkCStr++;
        }

        // Two bytes
        else if ((pkCStr[0] & 0xE0) == 0xC0)
        {
            CodePoint = ( ((pkCStr[0] & 0x1F) << 6) |
                          ((pkCStr[1] & 0x3F) << 0) );
            pkCStr += 2;
        }

        // Three bytes
        else if ((pkCStr[0] & 0xF0) == 0xE0)
        {
            CodePoint = ( ((pkCStr[0] & 0xF)  << 12) |
                          ((pkCStr[1] & 0x3F) <<  6) |
                          ((pkCStr[2] & 0x3F) <<  0) );
            pkCStr += 3;
        }

        // Four bytes
        else if ((pkCStr[0] & 0xF8) == 0xF0)
        {
            CodePoint = ( ((pkCStr[0] & 0x7)  << 18) |
                          ((pkCStr[1] & 0x3F) << 12) |
                          ((pkCStr[2] & 0x3F) <<  6) |
                          ((pkCStr[3] & 0x3F) <<  0) );
            pkCStr += 4;
        }

        // The 5/6-byte variants are not technically needed, as Unicode code points are limited to 21 bits
        // However they are here anyway for the sake of completeness

        // Five bytes
        else if ((pkCStr[0] & 0xFC) == 0xF8)
        {
            CodePoint = ( ((pkCStr[0] & 0x3)  << 24) |
                          ((pkCStr[1] & 0x3F) << 18) |
                          ((pkCStr[2] & 0x3F) << 12) |
                          ((pkCStr[3] & 0x3F) <<  6) |
                          ((pkCStr[4] & 0x3F) <<  0) );
            pkCStr += 5;
        }

        // Six bytes
        else if ((pkCStr[0] & 0xFE) == 0xFC)
        {
            CodePoint = ( ((pkCStr[0] & 0x1)  << 30) |
                          ((pkCStr[1] & 0x3F) << 24) |
                          ((pkCStr[2] & 0x3F) << 18) |
                          ((pkCStr[3] & 0x3F) << 12) |
                          ((pkCStr[4] & 0x3F) <<  6) |
                          ((pkCStr[5] & 0x3F) <<  0) );
            pkCStr += 6;
        }

        // Invalid?
        else
        {
            errorf("ToUTF16() encountered invalid UTF-8 data: 0x%02X", pkCStr[0]);
            return u"";
        }

        // Step 2: Check for errors
        // 0xD800 to 0xDFFF are invalid code points; they are reserved for UTF-16 surrogate encoding
        // 0x10FFFF is the largest defined code point
        if ( (CodePoint >= 0xD800 && CodePoint <= 0xDFFF) || (CodePoint > 0x10FFFF) )
        {
            errorf("ToUTF16() encountered invalid code point: 0x%X", CodePoint);
            return u"";
        }

        // Step 3: Re-encode as UTF-16
        // Two bytes
        if (CodePoint < 0x10000)
        {
            Out.Append( (char16_t) CodePoint );
        }
        // Four bytes
        else
        {
            CodePoint -= 0x10000;
            Out.Append( (char16_t) (0xD800 | ((CodePoint >> 10) & 0x3FF)) );
            Out.Append( (char16_t) (0xDC00 | ((CodePoint >>  0) & 0x3FF)) );
        }
    }

    Out.Shrink();
    return Out;
}

// ************ TWideString ************
uint32 TWideString::Hash32() const
{
    CFNV1A Hash(CFNV1A::k32Bit);
    Hash.HashData(Data(), Size() * sizeof(char16_t));
    return Hash.GetHash32();
}

uint64 TWideString::Hash64() const
{
    CFNV1A Hash(CFNV1A::k64Bit);
    Hash.HashData(Data(), Size() * sizeof(char16_t));
    return Hash.GetHash64();
}

TString TWideString::ToUTF8() const
{
    TString Out;
    Out.Reserve(Size());

    const char16_t* pkCStr = CString();

    while (pkCStr)
    {
        // Step 1: Get the code point
        uint32 CodePoint;

        // Two bytes
        if (pkCStr[0] <= 0xD7FF || pkCStr[0] >= 0xE000)
        {
            CodePoint = (uint32) pkCStr[0];
            pkCStr++;
        }
        // Four bytes
        else
        {
            CodePoint = ((pkCStr[0] - 0xD800) << 10) |
                        ((pkCStr[1] - 0xDC00));
            CodePoint += 0x10000;
            pkCStr += 2;
        }

        // Step 2: Check for errors
        // 0xD800 to 0xDFFF are invalid code points; they are reserved for UTF-16 surrogate encoding
        // 0x10FFFF is the largest defined code point
        if ( (CodePoint >= 0xD800 && CodePoint <= 0xDFFF) || (CodePoint > 0x10FFFF) )
        {
            errorf("ToUTF16() encountered invalid code point: 0x%X", CodePoint);
            return "";
        }

        // Step 3: Re-encode as UTF-8
        // One byte
        if (CodePoint <= 0x7F)
        {
            Out.Append( (char) CodePoint );
        }
        // Two bytes
        else if (CodePoint <= 0x7FF)
        {
            Out.Append( (char) (0xC0 | ((CodePoint >> 6) & 0x1F)) );
            Out.Append( (char) (0x80 | ((CodePoint >> 0) & 0x3F)) );
        }
        // Three bytes
        else if (CodePoint <= 0xFFFF)
        {
            Out.Append( (char) (0xE0 | ((CodePoint >> 12) & 0x0F)) );
            Out.Append( (char) (0x80 | ((CodePoint >>  6) & 0x3F)) );
            Out.Append( (char) (0x80 | ((CodePoint >>  0) & 0x3F)) );
        }
        // Four bytes
        else if (CodePoint <= 0x10FFFF)
        {
            Out.Append( (char) (0xF0 | ((CodePoint >> 18) & 0x07)) );
            Out.Append( (char) (0x80 | ((CodePoint >> 12) & 0x3F)) );
            Out.Append( (char) (0x80 | ((CodePoint >>  6) & 0x3F)) );
            Out.Append( (char) (0x80 | ((CodePoint >>  0) & 0x3F)) );
        }
    }

    Out.Shrink();
    return Out;
}
