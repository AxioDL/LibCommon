#ifndef AXIO_CFOURCC_H
#define AXIO_CFOURCC_H

#include "FileIO/IInputStream.h"
#include "FileIO/IOutputStream.h"
#include "Macros.h"
#include "TString.h"

#include <compare>
#include <cstdint>

// Note: All FourCC constants should be wrapped in this macro
#define FOURCC(Value) Value

class CFourCC
{
    uint32_t mFourCC = 0;

    static constexpr uint32_t FourCCFromText(std::string_view str)
    {
        return uint32_t(str[0]) << 24 | uint32_t(str[1]) << 16 | uint32_t(str[2]) << 8 | uint32_t(str[3]);
    }

public:
    // Constructors
    constexpr CFourCC() = default;
    constexpr explicit CFourCC(std::string_view txt) : mFourCC{FourCCFromText(txt)} {}
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

        for (size_t i = 0; i < 4; i++)
            reinterpret_cast<char*>(&Out.mFourCC)[i] = TString::CharToUpper(reinterpret_cast<const char*>(&mFourCC)[i]);

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

    [[nodiscard]] TString operator+(std::string_view text) const
    {
        return ToString() + TString(text);
    }

    [[nodiscard]] friend TString operator+(std::string_view text, const CFourCC& fourcc)
    {
        return TString(text) + fourcc.ToString();
    }

    CFourCC& operator=(std::string_view str)
    {
        mFourCC = FourCCFromText(str);
        return *this;
    }

    CFourCC& operator=(uint32_t Src)
    {
        mFourCC = Src;
        return *this;
    }

    [[nodiscard]] constexpr auto operator<=>(const CFourCC&) const noexcept = default;
};

#endif // AXIO_CFOURCC_H
