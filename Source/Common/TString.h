#ifndef AXIO_TSTRING_H
#define AXIO_TSTRING_H

#include "Common/Hash/CCRC32.h"
#include "Common/Hash/CFNV1A.h"
#include "Common/Macros.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <list>
#include <sstream>
#include <string>
#include <type_traits>

#include <fmt/format.h>

/**
 * This is a string class which is essentially a wrapper around std::basic_string.
 * The reason for this is because there are a lot of extremely common string operations
 * that are not supported by std::string.
 *
 * Most of the file contains an implementation for a template base class, TBasicString.
 * Afterwards we define the following subclasses/typedefs:
 *
 * - TString (char, utf-8)
 * - T16String (char16_t, utf-16)
 * - T32String (char32_t, utf-32)
 *
 * String types have functions for converting between each other. For these functions,
 * the above encoding conventions need to be respected for correct results.
 *
 * To convert to wchar_t*, enclose the string in the ToWChar() macro.
 */

// Helper macros for creating string literals of the correct char type. Internal use only! Invalid outside of this header!
#define LITERAL(Text) \
    (std::is_same_v<CharType, char16_t> ? (const CharType*) u##Text : \
    (std::is_same_v<CharType, char32_t> ? (const CharType*) U##Text : \
     Text ))

#define CHAR_LITERAL(Text) ((CharType) Text)

// ************ TBasicString ************
template<class _CharType, class _ListType>
class TBasicString
{
public:
    using CharType = _CharType;

protected:
    using _TString = TBasicString;
    using _TStdString = std::basic_string<_CharType>;
    using _TStdStringView = std::basic_string_view<_CharType>;
    using _TStringList = _ListType;

    _TStdString mInternalString{};

public:
    // Constructors
    TBasicString() = default;

    TBasicString(const CharType* pkText)
    {
        if (pkText)
            mInternalString = pkText;
    }

    TBasicString(const _TStdString& rkText)
        : mInternalString(rkText)
    {
    }

    TBasicString(_TStdString&& rkText)
        : mInternalString(std::move(rkText))
    {
    }

    explicit TBasicString(size_t Size)
        : mInternalString(Size, 0)
    {
    }

    explicit TBasicString(size_t Size, CharType Fill)
        : mInternalString(Size, Fill)
    {
    }

    explicit TBasicString(const CharType* pkText, size_t Length)
        : mInternalString(pkText, Length)
    {
    }

    explicit TBasicString(_TStdStringView rkText)
        : mInternalString(rkText)
    {
    }

    TBasicString(const TBasicString&) = default;
    TBasicString& operator=(const TBasicString&) = default;

    TBasicString(TBasicString&&) noexcept = default;
    TBasicString& operator=(TBasicString&&) noexcept = default;

    // Data Accessors
    const CharType* CString() const
    {
        return mInternalString.c_str();
    }

    CharType* Data()
    {
        return mInternalString.data();
    }

    const CharType* Data() const
    {
        return mInternalString.data();
    }

    CharType At(size_t Pos) const
    {
        assert(Size() > Pos);
        return mInternalString.at(Pos);
    }

    CharType Front() const
    {
        return (Size() > 0 ? mInternalString.front() : 0);
    }

    CharType Back() const
    {
        return (Size() > 0 ? mInternalString.back() : 0);
    }

    const CharType* Begin() const
    {
        return mInternalString.data();
    }

    const CharType* End() const
    {
        return mInternalString.data() + mInternalString.size();
    }

    size_t Size() const
    {
        return mInternalString.size();
    }

    size_t Length() const
    {
        return Size();
    }

    int64_t IndexOf(CharType Character, size_t Offset) const
    {
        return mInternalString.find_first_of(Character, Offset);
    }

    int64_t IndexOf(CharType Character) const
    {
        return IndexOf(Character, 0);
    }

    int64_t IndexOf(const CharType* pkCharacters, size_t Offset) const
    {
        return mInternalString.find_first_of(pkCharacters, Offset);
    }

    int64_t IndexOf(const CharType* pkCharacters) const
    {
        return IndexOf(pkCharacters, 0);
    }

    int64_t LastIndexOf(CharType Character) const
    {
        return mInternalString.find_last_of(Character);
    }

    int64_t LastIndexOf(const CharType* pkCharacters) const
    {
        return mInternalString.find_last_of(pkCharacters);
    }

    int64_t IndexOfPhrase(_TStdStringView rkStr, size_t Offset, bool CaseSensitive = true) const
    {
        if (Size() < rkStr.size())
            return -1;

        // Now loop from the offset provided by the user.
        size_t Pos = Offset;
        size_t LatestPossibleStart = Size() - rkStr.size();
        int64_t MatchStart = -1;
        int64_t Matched = 0;

        while (Pos < Size())
        {
            // If this character matches, increment Matched!
            const bool Match = CaseSensitive ? (mInternalString[Pos] == rkStr[Matched])
                                             : (CharToUpper(mInternalString[Pos]) == CharToUpper(rkStr[Matched]));

            if (Match)
            {
                Matched++;

                if (MatchStart == -1)
                    MatchStart = Pos;

                // If we matched the entire string, we can return.
                if (Matched == rkStr.size())
                    return MatchStart;
            }
            else
            {
                // If we didn't match, clear our existing match check.
                if (Matched > 0)
                {
                    Pos = MatchStart;
                    Matched = 0;
                    MatchStart = -1;
                }

                // Check if we're too far in to find another match.
                if (Pos > LatestPossibleStart)
                    break;
            }

            Pos++;
        }

        return -1;
    }

    int64_t IndexOfPhrase(_TStdStringView rkStr, bool CaseSensitive = true) const
    {
        return IndexOfPhrase(rkStr, 0, CaseSensitive);
    }

    // Modify String
    _TString SubString(size_t StartPos, size_t Length) const
    {
        return mInternalString.substr(StartPos, Length);
    }

    void Reserve(size_t Amount)
    {
        mInternalString.reserve(Amount);
    }

    void Shrink()
    {
        mInternalString.shrink_to_fit();
    }

    void Insert(size_t Pos, CharType Chr)
    {
        assert(Size() >= Pos);
        mInternalString.insert(Pos, 1, Chr);
    }

    void Insert(size_t Pos, const CharType* pkStr)
    {
        assert(Size() >= Pos);
        mInternalString.insert(Pos, pkStr);
    }

    void Insert(size_t Pos, const _TString& rkStr)
    {
        assert(Size() >= Pos);
        mInternalString.insert(Pos, rkStr.mInternalString);
    }

    void Remove(size_t Pos, size_t Len)
    {
#ifdef _DEBUG
        assert(Size() > Pos);
#endif
        mInternalString.erase(Pos, Len);
    }

    void Remove(const CharType* pkStr, bool CaseSensitive = false)
    {
        size_t InStrLen = CStringLength(pkStr);

        for (auto Idx = IndexOfPhrase(pkStr, CaseSensitive); Idx != -1; Idx = IndexOfPhrase(pkStr, Idx, CaseSensitive))
            Remove(Idx, InStrLen);
    }

    void Remove(CharType Chr)
    {
        std::erase_if(mInternalString, [Chr](auto c) { return c == Chr; });
    }

    void RemoveWhitespace()
    {
        std::erase_if(mInternalString, &IsWhitespace);
    }

    void Replace(const CharType* pkStr, const CharType *pkReplacement, bool CaseSensitive = false)
    {
        size_t Offset = 0;
        size_t InStrLen = CStringLength(pkStr);
        size_t ReplaceStrLen = CStringLength(pkReplacement);

        for (auto Idx = IndexOfPhrase(pkStr, CaseSensitive); Idx != -1; Idx = IndexOfPhrase(pkStr, Offset, CaseSensitive))
        {
            Remove(Idx, InStrLen);
            Insert(Idx, pkReplacement);
            Offset = Idx + ReplaceStrLen;
        }
    }

    void Replace(const _TString& rkStr, const _TString& rkReplacement, bool CaseSensitive)
    {
        Replace(rkStr.CString(), rkReplacement.CString(), CaseSensitive);
    }

    void Append(CharType Chr)
    {
        mInternalString.append(1, Chr);
    }

    void Append(const CharType* pkText)
    {
        mInternalString.append(pkText);
    }

    void Append(const _TString& rkStr)
    {
        mInternalString.append(rkStr.mInternalString);
    }

    void Prepend(CharType Chr)
    {
        Insert(0, Chr);
    }

    void Prepend(const CharType* pkText)
    {
        Insert(0, pkText);
    }

    void Prepend(const _TString& rkStr)
    {
        Insert(0, rkStr);
    }

    _TString ToUpper() const
    {
        // todo: doesn't handle accented characters
        _TString Out(Size());
        std::transform(begin(), end(), Out.begin(), &CharToUpper);
        return Out;
    }

    _TString ToLower() const
    {
        // todo: doesn't handle accented characters
        _TString Out(Size());
        std::transform(begin(), end(), Out.begin(), &CharToLower);
        return Out;
    }

    _TString Trimmed() const
    {
        size_t Start = _TStdString::npos;
        size_t End = _TStdString::npos;

        for (size_t iChar = 0; iChar < Size(); iChar++)
        {
            if (!IsWhitespace(mInternalString[iChar]))
            {
                Start = iChar;
                break;
            }
        }

        // If start is still -1 then there are no non-whitespace characters in this string. Return early.
        if (Start == _TStdString::npos)
            return _TString();

        for (int64_t iChar = Size() - 1; iChar >= 0; iChar--)
        {
            if (!IsWhitespace(mInternalString[iChar]))
            {
                End = static_cast<size_t>(iChar + 1);
                break;
            }
        }

        return SubString(Start, End - Start);
    }

    _TString Truncate(size_t Amount) const
    {
        return SubString(0, Amount);
    }

    _TString ChopFront(size_t Amount) const
    {
        if (Size() <= Amount)
            return _TString();

        return SubString(Amount, Size() - Amount);
    }

    _TString ChopBack(size_t Amount) const
    {
        if (Size() <= Amount)
            return _TString();

        return SubString(0, Size() - Amount);
    }

    int32_t ToInt32(int Base = 10) const
    {
        try {
            return (int32_t) std::stoul(mInternalString, nullptr, Base);
        }
        catch(...) {
            assert(false);
            return 0;
        }
    }

    int64_t ToInt64(int Base = 10) const
    {
        try {
            return (int64_t) std::stoull(mInternalString, nullptr, Base);
        }
        catch(...) {
            assert(false);
            return 0;
        }
    }

    void ToInt128(void* pOut, int Base = 16) const
    {
        try {
            // TODO: only works in base 16
            uint64_t Part1 = std::stoull(mInternalString.substr(0, 16), nullptr, Base);
            uint64_t Part2 = std::stoull(mInternalString.substr(16, 16), nullptr, Base);

            if constexpr (std::endian::native == std::endian::little)
            {
                Part1 = std::byteswap(Part1);
                Part2 = std::byteswap(Part2);
            }

            memcpy(static_cast<char*>(pOut) + 0, &Part1, 8);
            memcpy(static_cast<char*>(pOut) + 8, &Part2, 8);
        }
        catch(...) {
            assert(false);
        }
    }

    float ToFloat() const
    {
        return std::strtof(mInternalString.c_str(), nullptr);
    }

    const _TStdString& ToStdString() const
    {
        return mInternalString;
    }

    _TStringList Split(const CharType* pkTokens, bool KeepEmptyParts = false) const
    {
        _TStringList Out;
        size_t LastSplit = 0;

        // Iterate over all characters in the input string
        for (size_t iChr = 0; iChr < Length(); iChr++)
        {
            // Check whether this character is one of the user-provided tokens
            for (size_t iTok = 0; true; iTok++)
            {
                if (!pkTokens[iTok])
                    break;

                if (mInternalString[iChr] == pkTokens[iTok])
                {
                    // Token found - split string
                    if (iChr > LastSplit || KeepEmptyParts)
                        Out.push_back(SubString(LastSplit, iChr - LastSplit));

                    LastSplit = iChr + 1;
                    break;
                }
            }
        }

        // Add final string
        if (LastSplit != Length() || KeepEmptyParts)
        {
            const auto Len = Length() - LastSplit;
            Out.push_back(SubString(LastSplit, Len));
        }

        return Out;
    }

    void EnsureEndsWith(CharType Chr)
    {
        if (Back() != Chr)
            Append(Chr);
    }

    void EnsureEndsWith(const CharType* pkText)
    {
        if (!EndsWith(pkText))
            Append(pkText);
    }

    // Check String
    bool IsEmpty() const
    {
        return mInternalString.empty();
    }

    bool StartsWith(CharType Chr, bool CaseSensitive = true) const
    {
        if (IsEmpty())
            return false;

        return CaseSensitive ? Front() == Chr : CharToUpper(Front()) == CharToUpper(Chr);
    }

    bool StartsWith(_TStdStringView rkStr, bool CaseSensitive = true) const
    {
        if (Size() < rkStr.size())
            return false;

        if (CaseSensitive)
            return mInternalString.starts_with(rkStr);

        const auto tmp = _TStdStringView(mInternalString.data(), rkStr.size());
        return CaseInsensitiveCompare(tmp, rkStr);
    }

    bool EndsWith(CharType Chr, bool CaseSensitive = true) const
    {
        if (IsEmpty())
            return false;

        return CaseSensitive ? Back() == Chr : CharToUpper(Back()) == CharToUpper(Chr);
    }

    bool EndsWith(_TStdStringView rkStr, bool CaseSensitive = true) const
    {
        if (Size() < rkStr.size())
            return false;

        if (CaseSensitive)
            return mInternalString.ends_with(rkStr);

        const auto start = Size() - rkStr.size();
        const auto tmp = _TStdStringView(mInternalString.begin() + start, mInternalString.end());
        return CaseInsensitiveCompare(tmp, rkStr);
    }

    bool Contains(_TStdStringView Str, bool CaseSensitive = true) const
    {
        return IndexOfPhrase(Str, CaseSensitive) != -1;
    }

    bool Contains(CharType Chr) const
    {
        return IndexOf(Chr) != -1;
    }

    bool IsHexString(bool RequirePrefix = false, size_t Width = _TStdString::npos) const
    {
        const bool HasPrefix = StartsWith(LITERAL("0x"));

        // If we're required to match the prefix and prefix is missing, return false
        if (RequirePrefix && !HasPrefix)
            return false;

        _TStdStringView View = mInternalString;
        if (Width == _TStdString::npos)
        {
            // If the string has the 0x prefix, remove it
            if (HasPrefix)
                View.remove_prefix(2);

            // If the string is empty other than the prefix, then this is not a valid hex string
            if (View.empty())
                return false;

            // If we have a variable width then assign the width value to the string size
            Width = View.size();
        }
        // If the string starts with the prefix and the length matches the string, remove the prefix
        else if (View.size() == Width + 2 && HasPrefix)
        {
            View.remove_prefix(2);
        }

        // By this point, the string size and the width should match. If they don't, return false.
        if (View.size() != Width)
            return false;

        // Now we can finally check the actual string and make sure all the characters are valid hex characters.
        for (const auto Chr : View)
        {
            if (!((Chr >= CHAR_LITERAL('0')) && (Chr <= CHAR_LITERAL('9'))) &&
                !((Chr >= CHAR_LITERAL('a')) && (Chr <= CHAR_LITERAL('f'))) &&
                !((Chr >= CHAR_LITERAL('A')) && (Chr <= CHAR_LITERAL('F'))))
                return false;
        }

        return true;
    }

    bool CaseInsensitiveCompare(_TStdStringView other) const
    {
        return CaseInsensitiveCompare(*this, other);
    }
    static bool CaseInsensitiveCompare(_TStdStringView lhs, _TStdStringView rhs)
    {
        return std::ranges::equal(lhs, rhs,
                                  [](auto l, auto r) { return CharToUpper(l) == CharToUpper(r); });
    }

    // Hashing
    uint32_t Hash32() const
    {
        return CCRC32::StaticHashData(Data(), Size() * sizeof(CharType));
    }

    uint64_t Hash64() const
    {
        return CFNV1A::StaticHashData64(Data(), Size() * sizeof(CharType));
    }

    // Get Filename Components
    _TString GetFileDirectory() const
    {
        size_t EndPath = mInternalString.find_last_of(LITERAL("\\/"));
        return EndPath == _TStdString::npos ? _TString() : SubString(0, EndPath + 1);
    }

    _TString GetFileName(bool WithExtension = true) const
    {
        size_t EndPath = mInternalString.find_last_of(LITERAL("\\/")) + 1;

        if (WithExtension)
        {
            return SubString(EndPath, Size() - EndPath);
        }
        else
        {
            size_t EndName = mInternalString.find_last_of(CHAR_LITERAL('.'));
            return SubString(EndPath, EndName - EndPath);
        }
    }

    _TString GetFileExtension() const
    {
        size_t EndName = mInternalString.find_last_of(CHAR_LITERAL('.'));
        return EndName == _TStdString::npos ? _TString() : SubString(EndName + 1, Size() - EndName);
    }

    _TString GetFilePathWithoutExtension() const
    {
        size_t EndName = mInternalString.find_last_of(CHAR_LITERAL('.'));
        return EndName == _TStdString::npos ? *this : SubString(0, EndName);
    }

    _TString GetParentDirectoryPath(_TStdStringView rkParentDirName, bool CaseSensitive = true)
    {
        int64_t IdxA = 0;
        int64_t IdxB = IndexOf(LITERAL("\\/"));
        if (IdxB == -1)
            return _TString();

        while (IdxB != -1)
        {
            _TString DirName = SubString(IdxA, IdxB - IdxA);

            if (CaseSensitive ? (DirName == rkParentDirName) : (DirName.CaseInsensitiveCompare(rkParentDirName)))
                return Truncate(IdxB + 1);

            IdxA = IdxB + 1;
            IdxB = IndexOf(LITERAL("\\/"), IdxA);
        }

        return _TString();
    }

    // Operators


    _TString& operator=(CharType Char)
    {
        mInternalString = Char;
        return *this;
    }

    _TString& operator=(const CharType* pkText)
    {
        mInternalString = pkText;
        return *this;
    }

    _TString& operator=(_TStdStringView view)
    {
        mInternalString = view;
        return *this;
    }

    CharType& operator[](size_t Pos)
    {
        return mInternalString[Pos];
    }

    const CharType& operator[](size_t Pos) const
    {
        return mInternalString[Pos];
    }

    // Intentionally non-explicit
    operator _TStdStringView() const
    {
        return mInternalString;
    }

    const CharType* operator*() const
    {
        return CString();
    }

    _TString operator+(CharType Other) const
    {
        return _TString(mInternalString + Other);
    }

    _TString operator+(const CharType* pkOther) const
    {
        return _TString(mInternalString + pkOther);
    }

    _TString operator+(const _TString& rkOther) const
    {
        return _TString(mInternalString + rkOther.mInternalString);
    }

    _TString& operator+=(CharType Other)
    {
        mInternalString += Other;
        return *this;
    }

    _TString& operator+=(const CharType* pkOther)
    {
        mInternalString += pkOther;
        return *this;
    }

    _TString& operator+=(const _TString& rkOther)
    {
        mInternalString += rkOther.mInternalString;
        return *this;
    }

    _TString& operator+=(_TStdStringView view)
    {
        mInternalString += view;
        return *this;
    }

    _TString operator/(const CharType* pkOther) const
    {
        // Append operator that ensures a slash separates the element being appended
        // Useful for constructing filesystem paths
        _TString Out = *this;

        if (Back() != '/' && Back() != '\\')
        {
            Out.Append('/');
        }

        return Out.operator+(pkOther);
    }

    _TString operator/(const _TString& kOther) const
    {
        return operator/(*kOther);
    }

    friend _TString operator/(const CharType* pkLeft, const _TString& rkRight)
    {
        return  _TString(pkLeft).operator/(*rkRight);
    }

    friend _TString operator+(CharType Left, const _TString& rkRight)
    {
        return _TString(Left + rkRight.mInternalString);
    }

    friend _TString operator+(const CharType* pkLeft, const _TString& rkRight)
    {
        return _TString(pkLeft + rkRight.mInternalString);
    }

    friend _TString operator+(const _TStdString& rkLeft, const _TString& rkRight)
    {
        return _TString(rkLeft + rkRight.mInternalString);
    }

    bool operator==(CharType Other) const
    {
        return Size() == 1 && At(0) == Other;
    }

    bool operator==(const CharType *pkText) const
    {
        return CompareCStrings(pkText, CString());
    }

    bool operator==(const _TString& rkOther) const
    {
        return mInternalString == rkOther.mInternalString;
    }

    friend bool operator==(CharType Other, const _TString& rkString)
    {
        return (rkString == Other);
    }

    friend bool operator==(const CharType *pkText, const _TString& rkString)
    {
        return (rkString == pkText);
    }

    friend bool operator==(const _TStdString& rkStringA, const _TString& rkStringB)
    {
        return (rkStringB == rkStringA);
    }

    friend bool operator==(const _TString& str, _TStdStringView view)
    {
        return str.mInternalString == view;
    }

    friend bool operator==(_TStdStringView view, const _TString& str)
    {
        return view == str.mInternalString;
    }

    bool operator!=(CharType Other) const
    {
        return (!(*this == Other));
    }

    bool operator!=(const CharType *pkText) const
    {
        return (!(*this == pkText));
    }

    bool operator!=(const _TString& rkOther) const
    {
        return !(*this == rkOther);
    }

    friend bool operator!=(CharType Other, const _TString& rkString)
    {
        return (rkString != Other);
    }

    friend bool operator!=(const CharType *pkText, const _TString& rkString)
    {
        return (rkString != pkText);
    }

    friend bool operator!=(const _TStdString& rkStringA, const _TString& rkStringB)
    {
        return (rkStringB != rkStringA);
    }

    friend bool operator!=(const _TString& str, _TStdStringView view)
    {
        return str.mInternalString != view;
    }

    friend bool operator!=(_TStdStringView view, const _TString& str)
    {
        return view != str.mInternalString;
    }

    bool operator<(const CharType* pkText) const
    {
        return (mInternalString < pkText);
    }

    bool operator<(const _TString& rkOther) const
    {
        return mInternalString < rkOther.mInternalString;
    }

    friend bool operator<(const CharType* pkText, const _TString& rkString)
    {
        return (rkString > pkText);
    }

    friend bool operator<(const _TStdString& rkStringA, const _TString& rkStringB)
    {
        return (rkStringB > rkStringA);
    }

    friend bool operator<(const _TString& str, _TStdStringView view)
    {
        return str.mInternalString < view;
    }

    friend bool operator<(_TStdStringView view, const _TString& str)
    {
        return view < str.mInternalString;
    }

    bool operator<=(const CharType* pkText) const
    {
        return (mInternalString <= pkText);
    }

    bool operator<=(const _TString& rkOther) const
    {
        return mInternalString <= rkOther.mInternalString;
    }

    friend bool operator<=(const CharType* pkText, const _TString& rkString)
    {
        return (rkString >= pkText);
    }

    friend bool operator<=(const _TStdString& rkStringA, const _TString& rkStringB)
    {
        return (rkStringB >= rkStringA);
    }

    friend bool operator<=(const _TString& str, _TStdStringView view)
    {
        return str.mInternalString <= view;
    }

    friend bool operator<=(_TStdStringView view, const _TString& str)
    {
        return view <= str.mInternalString;
    }

    bool operator>(const CharType* pkText) const
    {
        return (mInternalString > pkText);
    }

    bool operator>(const _TString& rkOther) const
    {
        return (mInternalString > rkOther.mInternalString);
    }

    friend bool operator>(const CharType* pkText, const _TString& rkString)
    {
        return (rkString < pkText);
    }

    friend bool operator>(const _TStdString& rkStringA, const _TString& rkStringB)
    {
        return (rkStringB < rkStringA);
    }

    friend bool operator>(const _TString& str, _TStdStringView view)
    {
        return str.mInternalString > view;
    }

    friend bool operator>(_TStdStringView view, const _TString& str)
    {
        return view > str.mInternalString;
    }

    bool operator>=(const CharType* pkText) const
    {
        return (mInternalString >= pkText);
    }

    bool operator>=(const _TString& rkOther) const
    {
        return mInternalString >= rkOther.mInternalString;
    }

    friend bool operator>=(const CharType* pkText, const _TString& rkString)
    {
        return (rkString <= pkText);
    }

    friend bool operator>=(const _TStdString& rkStringA, const _TString& rkStringB)
    {
        return (rkStringB <= rkStringA);
    }

    friend bool operator>=(const _TString& str, _TStdStringView view)
    {
        return str.mInternalString >= view;
    }

    friend bool operator>=(_TStdStringView view, const _TString& str)
    {
        return view >= str.mInternalString;
    }

    friend std::ostream& operator<<(std::ostream& rStream, const _TString& rkString)
    {
        rStream << rkString.mInternalString;
        return rStream;
    }

    friend std::istream& operator>>(std::istream& rStream, const _TString& rkString)
    {
        rStream >> rkString.mInternalString;
        return rStream;
    }

    auto begin() { return mInternalString.begin(); }
    auto begin() const { return mInternalString.begin(); }

    auto end() { return mInternalString.end(); }
    auto end() const { return mInternalString.end(); }

    auto rbegin() { return mInternalString.rbegin(); }
    auto rbegin() const { return mInternalString.rbegin(); }

    auto rend() { return mInternalString.rend(); }
    auto rend() const { return mInternalString.rend(); }

    auto cbegin() const { return mInternalString.cbegin(); }
    auto cend() const { return mInternalString.cend(); }

    auto crbegin() const { return mInternalString.crbegin(); }
    auto crend() const { return mInternalString.crend(); }

    // Static
    static _TString Format(const CharType *pkFmt, ...)
    {
        // Probably better to rewrite this at some point for better error handling + avoiding all the C-style syntax
        constexpr int kBufferSize = 4096;
        CharType StringBuffer[kBufferSize];

        std::va_list Args;
        va_start(Args, pkFmt);

        int size = 0;
        if constexpr (std::is_same_v<CharType, char>)
            size = vsnprintf(StringBuffer, kBufferSize, pkFmt, Args);
        else
            size = vswprintf(StringBuffer, kBufferSize, pkFmt, Args);

        va_end(Args);
        return _TString(StringBuffer, static_cast<size_t>(size));
    }

    static _TString FromInt32(int32_t Value, int Width = 0, int Base = 16)
    {
        std::basic_ostringstream<CharType> SStream;
        SStream << std::setbase(Base) << std::setw(Width) << std::setfill(CHAR_LITERAL('0')) << Value;
        return SStream.str();
    }

    static _TString FromInt64(int64_t Value, int Width = 0, int Base = 16)
    {
        std::basic_ostringstream<CharType> SStream;
        SStream << std::setbase(Base) << std::setw(Width) << std::setfill(CHAR_LITERAL('0')) << Value;
        return SStream.str();
    }

    static _TString FromFloat(float Value, int MinDecimals = 1, bool Scientific = false)
    {
        _TString Out = _TString::Format(Scientific ? "%.8g" : "%f", Value);

        // Make sure we have the right number of decimals
        int64_t DecIdx = Out.IndexOf(CHAR_LITERAL('.'));

        if (DecIdx == -1 && MinDecimals > 0)
        {
            DecIdx = Out.Size();
            Out.Append(CHAR_LITERAL('.'));
        }

        int64_t NumZeroes = (DecIdx == -1 ? 0 : Out.Size() - (DecIdx + 1));

        // Add extra zeroes to meet the minimum decimal count
        if (NumZeroes < MinDecimals)
        {
            for (int64_t iDec = 0; iDec < (MinDecimals - NumZeroes); iDec++)
                Out.Append(CHAR_LITERAL('0'));
        }
        // Remove unnecessary trailing zeroes from the end of the string
        else if (NumZeroes > MinDecimals)
        {
            while (Out.Back() == CHAR_LITERAL('0') && NumZeroes > MinDecimals && NumZeroes > 0)
            {
                Out = Out.ChopBack(1);
                NumZeroes--;
            }

            // Remove decimal point
            if (NumZeroes == 0)
                Out = Out.ChopBack(1);
        }

        return Out;
    }

    static _TString FileSizeString(int64_t Size, int NumDecimals = 2)
    {
        _TString Out;
        _TString Type;

        if (Size < 100)
        {
            return FromInt64(Size, 0, 10) + LITERAL(" bytes");
        }
        else if (Size < 1000000)
        {
            Out = FromFloat(Size / 1000.f, NumDecimals);
            Type = LITERAL("KB");
        }
        else if (Size < 1000000000)
        {
            Out = FromFloat(Size / 1000000.f, NumDecimals);
            Type = LITERAL("MB");
        }
        else
        {
            Out = FromFloat(Size / 1000000000.f, NumDecimals);
            Type = LITERAL("GB");
        }

        const size_t DecCount = Out.Size() - (Out.IndexOf(CHAR_LITERAL('.')) + 1);
        if (DecCount > static_cast<size_t>(NumDecimals))
            Out = Out.ChopBack(DecCount - NumDecimals);

        return Out + Type;
    }

    static _TString HexString(uint8_t Num, int Width = 8, bool AddPrefix = true, bool Uppercase = true)
    {
        return HexString(uint64_t{Num}, Width, AddPrefix, Uppercase);
    }

    static _TString HexString(uint16_t Num, int Width = 8, bool AddPrefix = true, bool Uppercase = true)
    {
        return HexString(uint64_t{Num}, Width, AddPrefix, Uppercase);
    }

    static _TString HexString(uint32_t Num, int Width = 8, bool AddPrefix = true, bool Uppercase = true)
    {
        return HexString(uint64_t{Num}, Width, AddPrefix, Uppercase);
    }

    static _TString HexString(uint64_t Num, int Width = 16, bool AddPrefix = true, bool Uppercase = true)
    {
        std::basic_ostringstream<CharType> SStream;
        SStream << std::hex << std::setw(Width) << std::setfill('0') << Num;

        _TString Str = std::move(SStream).str();
        if (Uppercase)
            Str = Str.ToUpper();
        if (AddPrefix)
            Str.Prepend(LITERAL("0x"));
        return Str;
    }

    static constexpr bool CompareCStrings(const CharType* pkA, const CharType* pkB)
    {
        // Replacement for strcmp so we can compare any CharType
        while (true)
        {
            if (*pkA != *pkB)
                return false;
            if (*pkA == 0 || *pkB == 0)
                return true;

            ++pkA;
            ++pkB;
        }
    }

    static constexpr size_t CStringLength(const CharType* pkStr)
    {
        return std::char_traits<CharType>::length(pkStr);
    }

    static CharType CharToLower(CharType Chr)
    {
        // todo: doesn't handle accented characters
        return (Chr >= CHAR_LITERAL('A') && Chr <= CHAR_LITERAL('Z')) ? Chr + 0x20 : Chr;
    }

    static CharType CharToUpper(CharType Chr)
    {
        // todo: doesn't handle accented characters
        return (Chr >= CHAR_LITERAL('a') && Chr <= CHAR_LITERAL('z')) ? Chr - 0x20 : Chr;
    }

    static bool IsVowel(CharType Chr)
    {
        Chr = CharToUpper(Chr);
        return (Chr == 'A' ||
                Chr == 'E' ||
                Chr == 'I' ||
                Chr == 'O' ||
                Chr == 'U');
    }

    static bool IsWhitespace(CharType Chr)
    {
        return ( (Chr == CHAR_LITERAL('\t')) ||
                 (Chr == CHAR_LITERAL('\n')) ||
                 (Chr == CHAR_LITERAL('\v')) ||
                 (Chr == CHAR_LITERAL('\f')) ||
                 (Chr == CHAR_LITERAL('\r')) ||
                 (Chr == CHAR_LITERAL(' '))  );
    }

    static bool IsNumerical(CharType Chr)
    {
        return (Chr >= CHAR_LITERAL('0') && Chr <= CHAR_LITERAL('9'));
    }
};

#undef LITERAL
#undef CHAR_LITERAL

// ************ TString ************
class TString : public TBasicString<char, std::list<TString>>
{
public:
    using BaseClass = TBasicString;

    TString() = default;
    TString(const BaseClass& kIn) : BaseClass(kIn) {}
    using BaseClass::BaseClass;

    void AppendCodePoint(uint32_t CodePoint);
    class T16String ToUTF16() const;
    class T32String ToUTF32() const;
};

// ************ T16String ************
class T16String : public TBasicString<char16_t, std::list<T16String>>
{
public:
    using BaseClass = TBasicString;

    T16String() = default;
    T16String(const BaseClass& kIn) : BaseClass(kIn) {}
    using BaseClass::BaseClass;

    void AppendCodePoint(uint32_t CodePoint);
    class TString ToUTF8() const;
    class T32String ToUTF32() const;
};

// ************ T32String ************
class T32String : public TBasicString<char32_t, std::list<T32String>>
{
public:
    using BaseClass = TBasicString;

    T32String() = default;
    T32String(const BaseClass& kIn) : BaseClass(kIn) {}
    using BaseClass::BaseClass;

    void AppendCodePoint(uint32_t CodePoint);
    class TString ToUTF8() const;
    class T16String ToUTF16() const;
};

// ************ CToWChar ************
#define WCHAR_IS_16BIT WIN32

/** Class for converting strings to a wchar_t array.
 *  Generally meant for passing to functions; don't keep it resident.
 */
class CToWChar
{
#if WCHAR_IS_16BIT
    const T16String* mpStringPtr;
    T16String mConvertedString;
#else
    const T32String* mpStringPtr;
    T32String mConvertedString;
#endif

public:
#if WCHAR_IS_16BIT
    CToWChar(const TString& kInString)
        : mpStringPtr(nullptr)
        , mConvertedString( kInString.ToUTF16() )
    {}
    CToWChar(const T16String& kInString)
        : mpStringPtr(&kInString)
    {}
    CToWChar(const T32String& kInString)
        : mpStringPtr(nullptr)
        , mConvertedString( kInString.ToUTF16() )
    {}
#else
    CToWChar(const TString& kInString)
        : mpStringPtr(nullptr)
        , mConvertedString( kInString.ToUTF32() )
    {}
    CToWChar(const T16String& kInString)
        : mpStringPtr(nullptr)
        , mConvertedString( kInString.ToUTF32() )
    {}
    CToWChar(const T32String& kInString)
        : mpStringPtr(&kInString)
    {}
#endif

    const wchar_t* Decay() const
    {
        return reinterpret_cast<const wchar_t*>( mpStringPtr ? **mpStringPtr : *mConvertedString );
    }
    const wchar_t* operator*() const
    {
        return Decay();
    }
};

#define ToWChar *CToWChar

// ************ Aliases ************
using TStringList = std::list<TString>;
using T16StringList = std::list<T16String>;
using T32StringList = std::list<T32String>;

template <>
struct fmt::formatter<TBasicString<char, TStringList>> : fmt::formatter<std::string_view>
{
    auto format(const TBasicString<char, TStringList>& str, fmt::format_context& ctx) const {
        return fmt::formatter<std::string_view>::format(str.ToStdString(), ctx);
    }
};
template <>
struct fmt::formatter<TString> : fmt::formatter<std::string_view>
{
    auto format(const TString& str, fmt::format_context& ctx) const {
        return fmt::formatter<std::string_view>::format(str.ToStdString(), ctx);
    }
};

#endif // AXIO_TSTRING_H
