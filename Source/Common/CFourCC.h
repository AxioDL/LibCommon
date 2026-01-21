#ifndef AXIO_CFOURCC_H
#define AXIO_CFOURCC_H

#include "FileIO/IInputStream.h"
#include "FileIO/IOutputStream.h"
#include "Macros.h"
#include "TString.h"

#include <cstdint>

#define FOURCC_FROM_TEXT(Text) (Text[0] << 24 | Text[1] << 16 | Text[2] << 8 | Text[3])

// Note: All FourCC constants should be wrapped in this macro
#define FOURCC(Value) Value

class CFourCC
{
    // Note: mFourCC_Chars isn't used much due to endianness.
    union
    {
        uint32_t mFourCC = 0;
        char mFourCC_Chars[4];
    };

public:
    // Constructors
    constexpr CFourCC() = default;
    constexpr explicit CFourCC(const char* pkSrc) : mFourCC{static_cast<uint32_t>(FOURCC_FROM_TEXT(pkSrc))} {}
    explicit CFourCC(const TString& rkSrc)
    {
        ASSERT(rkSrc.Length() == 4);
        mFourCC = static_cast<uint32_t>(FOURCC_FROM_TEXT(rkSrc));
    }
    constexpr CFourCC(uint32_t Src) : mFourCC{Src} {}
    explicit CFourCC(IInputStream& rSrc) { Read(rSrc); }

    // Functionality
    void Read(IInputStream& rInput)
    {
        mFourCC = rInput.ReadU32();
        if (rInput.GetEndianness() == std::endian::little)
            Reverse();
    }

    void Write(IOutputStream& rOutput) const
    {
        uint32_t Val = mFourCC;
        if (rOutput.GetEndianness() == std::endian::little)
            Val = std::byteswap(Val);
        rOutput.WriteU32(Val);
    }

    [[nodiscard]] constexpr uint32_t ToU32() const
    {
        return mFourCC;
    }

    [[nodiscard]] TString ToString() const
    {
        char CharArray[4] = {
            (char) ((mFourCC >> 24) & 0xFF),
            (char) ((mFourCC >> 16) & 0xFF),
            (char) ((mFourCC >>  8) & 0xFF),
            (char) ((mFourCC >>  0) & 0xFF)
        };

        return TString(CharArray, 4);
    }

    [[nodiscard]] CFourCC ToUpper() const
    {
        CFourCC Out;

        for (size_t iChr = 0; iChr < 4; iChr++)
            Out.mFourCC_Chars[iChr] = TString::CharToUpper(mFourCC_Chars[iChr]);

        return CFourCC(Out);
    }

    void Reverse()
    {
        mFourCC = std::byteswap(mFourCC);
    }

    // Operators
    [[nodiscard]] char& operator[](int Index)
    {
        ASSERT(Index >= 0 && Index < 4);
        if constexpr (std::endian::native == std::endian::little)
            Index = 3 - Index;

        return reinterpret_cast<char*>(&mFourCC)[Index];
    }

    [[nodiscard]] const char& operator[](int Index) const
    {
        ASSERT(Index >= 0 && Index < 4);
        if constexpr (std::endian::native == std::endian::little)
            Index = 3 - Index;

        return reinterpret_cast<const char*>(&mFourCC)[Index];
    }

    [[nodiscard]] TString operator+(const char *pkText) const
    {
        return ToString() + pkText;
    }

    [[nodiscard]]  TString operator+(const TString& rkStr) const
    {
        return ToString() + rkStr;
    }

    [[nodiscard]] friend TString operator+(const char *pkText, const CFourCC& rkFourCC)
    {
        return pkText + rkFourCC.ToString();
    }

    [[nodiscard]] friend TString operator+(const TString& rkStr, const CFourCC& rkFourCC)
    {
        return rkStr + rkFourCC.ToString();
    }

    CFourCC& operator=(const char* pkSrc)
    {
        mFourCC = static_cast<uint32_t>(FOURCC_FROM_TEXT(pkSrc));
        return *this;
    }

    CFourCC& operator=(const TString& rkSrc)
    {
        mFourCC = static_cast<uint32_t>(FOURCC_FROM_TEXT(rkSrc));
        return *this;
    }

    CFourCC& operator=(uint32_t Src)
    {
        mFourCC = Src;
        return *this;
    }

    [[nodiscard]] bool operator==(const CFourCC& rkOther) const    { return mFourCC == rkOther.mFourCC;                    }
    [[nodiscard]] bool operator!=(const CFourCC& rkOther) const    { return mFourCC != rkOther.mFourCC;                    }
    [[nodiscard]] bool operator> (const CFourCC& rkOther) const    { return mFourCC >  rkOther.mFourCC;                    }
    [[nodiscard]] bool operator>=(const CFourCC& rkOther) const    { return mFourCC >= rkOther.mFourCC;                    }
    [[nodiscard]] bool operator< (const CFourCC& rkOther) const    { return mFourCC <  rkOther.mFourCC;                    }
    [[nodiscard]] bool operator<=(const CFourCC& rkOther) const    { return mFourCC <= rkOther.mFourCC;                    }
};

#endif // AXIO_CFOURCC_H
