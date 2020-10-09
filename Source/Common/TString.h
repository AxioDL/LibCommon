#ifndef TSTRING_H
#define TSTRING_H

#include "BasicTypes.h"
#include "FileIO/IOUtil.h"
#include "Hash/CCRC32.h"
#include "Hash/CFNV1A.h"
#include "Macros.h"

#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <list>
#include <sstream>
#include <string>
#include <vector>

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
    (typeid(CharType) == typeid(char16_t) ? (const CharType*) u##Text : \
    (typeid(CharType) == typeid(char32_t) ? (const CharType*) U##Text : \
     Text ))

#define CHAR_LITERAL(Text) (CharType) Text

// ************ TBasicString ************
template<class _CharType, class _ListType>
class TBasicString
{
public:
    using CharType = _CharType;

protected:
    using _TString = TBasicString<_CharType, _ListType>;
    using _TStdString = std::basic_string<_CharType>;
    using _TStdStringView = std::basic_string_view<_CharType>;
    using _TStringList = _ListType;

    _TStdString mInternalString{};

public:
    // Constructors
    TBasicString() = default;

    TBasicString(size_t Size)
        : mInternalString(Size, 0)
    {
    }

    TBasicString(size_t Size, CharType Fill)
        : mInternalString(Size, Fill)
    {
    }

    TBasicString(const CharType* pkText)
    {
        if (pkText)
            mInternalString = pkText;
    }

    TBasicString(const CharType* pkText, size_t Length)
        : mInternalString(pkText, Length)
    {
    }

    TBasicString(const _TStdString& rkText)
        : mInternalString(rkText)
    {
    }

    explicit TBasicString(_TStdStringView rkText)
        : mInternalString(rkText)
    {
    }

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
        if (Size() <= Pos)
        {
            errorf("Invalid position passed to TBasicString::At()");
            return 0;
        }

        return mInternalString.at(Pos);
    }

    CharType Front() const
    {
        return (Size() > 0 ? mInternalString[0] : 0);
    }

    CharType Back() const
    {
        return (Size() > 0 ? mInternalString[Size() - 1] : 0);
    }

    const CharType* Begin() const
    {
        return mInternalString.data();
    }

    const CharType* End() const
    {
        return mInternalString.data() + mInternalString.size();
    }

    uint Size() const
    {
        return mInternalString.size();
    }

    uint Length() const
    {
        return Size();
    }

    int IndexOf(CharType Character, size_t Offset) const
    {
        size_t Pos = mInternalString.find_first_of(Character, Offset);
        return (Pos == _TStdString::npos ? -1 : (int) Pos);
    }

    int IndexOf(CharType Character) const
    {
        return IndexOf(Character, 0);
    }

    int IndexOf(const CharType* pkCharacters, size_t Offset) const
    {
        size_t Pos = mInternalString.find_first_of(pkCharacters, Offset);
        return (Pos == _TStdString::npos ? -1 : (int) Pos);
    }

    int IndexOf(const CharType* pkCharacters) const
    {
        return IndexOf(pkCharacters, 0);
    }

    int LastIndexOf(CharType Character) const
    {
        size_t Pos = mInternalString.find_last_of(Character);
        return (Pos == _TStdString::npos ? -1 : (int) Pos);
    }

    int LastIndexOf(const CharType* pkCharacters) const
    {
        size_t Pos = mInternalString.find_last_of(pkCharacters);

        if (Pos == _TStdString::npos)
            return -1;
        else
            return (int) Pos;
    }

    int IndexOfPhrase(_TStdStringView rkStr, size_t Offset, bool CaseSensitive = true) const
    {
        if (Size() < rkStr.size())
            return -1;

        // Now loop from the offset provided by the user.
        size_t Pos = Offset;
        size_t LatestPossibleStart = Size() - rkStr.size();
        int MatchStart = -1;
        int Matched = 0;

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

    int IndexOfPhrase(_TStdStringView rkStr, bool CaseSensitive = true) const
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
        if (Size() < Pos)
        {
            errorf("Invalid position passed to TBasicString::Insert()");
            return;
        }

        mInternalString.insert(Pos, 1, Chr);
    }

    void Insert(size_t Pos, const CharType* pkStr)
    {
        if (Size() < Pos)
        {
            errorf("Invalid position passed to TBasicString::Insert()");
            return;
        }

        mInternalString.insert(Pos, pkStr);
    }

    void Insert(size_t Pos, const _TString& rkStr)
    {
        Insert(Pos, rkStr.CString());
    }

    void Remove(size_t Pos, uint Len)
    {
#ifdef _DEBUG
        if (Size() <= Pos)
        {
            errorf("Invalid position passed to TBasicString::Remove()");
            return;
        }
#endif
        mInternalString.erase(Pos, Len);
    }

    void Remove(const CharType* pkStr, bool CaseSensitive = false)
    {
        uint InStrLen = CStringLength(pkStr);

        for (int Idx = IndexOfPhrase(pkStr, CaseSensitive); Idx != -1; Idx = IndexOfPhrase(pkStr, Idx, CaseSensitive))
            Remove(Idx, InStrLen);
    }

    void Remove(CharType Chr)
    {
        for (int Idx = IndexOf(Chr); Idx != -1; Idx = IndexOf(Chr, Idx))
            Remove(Idx, 1);
    }

    void RemoveWhitespace()
    {
        for (size_t Idx = 0; Idx < Size(); Idx++)
        {
            if (IsWhitespace(mInternalString[Idx]))
            {
                Remove(Idx, 1);
                Idx--;
            }
        }
    }

    void Replace(const CharType* pkStr, const CharType *pkReplacement, bool CaseSensitive = false)
    {
        uint Offset = 0;
        uint InStrLen = CStringLength(pkStr);
        uint ReplaceStrLen = CStringLength(pkReplacement);

        for (int Idx = IndexOfPhrase(pkStr, CaseSensitive); Idx != -1; Idx = IndexOfPhrase(pkStr, Offset, CaseSensitive))
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
        mInternalString.append(rkStr.CString());
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
        _TString Out(Size());

        for (uint iChar = 0; iChar < Size(); iChar++)
            Out[iChar] = CharToUpper( At(iChar) );

        return Out;
    }

    _TString ToLower() const
    {
        // todo: doesn't handle accented characters
        _TString Out(Size());

        for (uint iChar = 0; iChar < Size(); iChar++)
            Out[iChar] = CharToLower( At(iChar) );

        return Out;
    }

    _TString Trimmed() const
    {
        int Start = -1, End = -1;

        for (uint iChar = 0; iChar < Size(); iChar++)
        {
            if (!IsWhitespace(mInternalString[iChar]))
            {
                Start = iChar;
                break;
            }
        }

        // If start is still -1 then there are no non-whitespace characters in this string. Return early.
        if (Start == -1)
            return _TString();

        for (int iChar = Size() - 1; iChar >= 0; iChar--)
        {
            if (!IsWhitespace(mInternalString[iChar]))
            {
                End = iChar + 1;
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

    int32 ToInt32(int Base = 10) const
    {
        try {
            return (int32) std::stoul(mInternalString, nullptr, Base);
        }
        catch(...) {
            errorf("ToInt32 failed (input: %s)", **this);
            return 0;
        }
    }

    int64 ToInt64(int Base = 10) const
    {
        try {
            return (int64) std::stoull(mInternalString, nullptr, Base);
        }
        catch(...) {
            errorf("ToInt64 failed (input: %s)", **this);
            return 0;
        }
    }

    void ToInt128(void* pOut, int Base = 16) const
    {
        try {
            // TODO: only works in base 16
            uint64 Part1 = std::stoull(mInternalString.substr(0, 16), nullptr, Base);
            uint64 Part2 = std::stoull(mInternalString.substr(16, 16), nullptr, Base);

            if constexpr (EEndian::SystemEndian == EEndian::LittleEndian)
            {
                SwapBytes(Part1);
                SwapBytes(Part2);
            }

            memcpy( ((char*) pOut) + 0, &Part1, 8);
            memcpy( ((char*) pOut) + 8, &Part2, 8);
        }
        catch(...) {
            errorf("ToUint128 failed (input: %s)", **this);
            return;
        }
    }

    float ToFloat() const
    {
        return std::strtof(mInternalString.c_str(), nullptr);
    }

    _TStdString ToStdString() const
    {
        return mInternalString;
    }

    _TStringList Split(const CharType* pkTokens, bool KeepEmptyParts = false) const
    {
        _TStringList Out;
        uint LastSplit = 0;

        // Iterate over all characters in the input string
        for (uint iChr = 0; iChr < Length(); iChr++)
        {
            // Check whether this character is one of the user-provided tokens
            for (uint iTok = 0; true; iTok++)
            {
                if (!pkTokens[iTok]) break;

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
            const uint Len = Length() - LastSplit;
            Out.push_back(SubString(LastSplit, Len >= Length() ? Length() - 1 : Len));
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
        return Size() == 0;
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

        _TString SubStr = SubString(0, rkStr.size());
        return CaseSensitive ? SubStr == rkStr : SubStr.CaseInsensitiveCompare(rkStr);
    }

    bool EndsWith(CharType Chr, bool CaseSensitive = true) const
    {
        if (IsEmpty())
            return false;

        return CaseSensitive ? Back() == Chr : CharToUpper(Back()) == CharToUpper(Chr);
    }

    bool EndsWith(const _TStdStringView rkStr, bool CaseSensitive = true) const
    {
        if (Size() < rkStr.size())
            return false;

        _TString SubStr = SubString(Size() - rkStr.size(), rkStr.size());
        return CaseSensitive ? SubStr == rkStr : SubStr.CaseInsensitiveCompare(rkStr);
    }

    bool Contains(_TStdStringView Str, bool CaseSensitive = true) const
    {
        return IndexOfPhrase(Str, CaseSensitive) != -1;
    }

    bool Contains(CharType Chr) const
    {
        return IndexOf(Chr) != -1;
    }

    bool IsHexString(bool RequirePrefix = false, int Width = -1) const
    {
        _TString Str(*this);
        bool HasPrefix = Str.StartsWith(LITERAL("0x"));

        // If we're required to match the prefix and prefix is missing, return false
        if (RequirePrefix && !HasPrefix)
            return false;

        if (Width == -1)
        {
            // If the string has the 0x prefix, remove it
            if (HasPrefix)
                Str = Str.ChopFront(2);

            // If the string is empty other than the prefix, then this is not a valid hex string
            if (Str.IsEmpty())
                return false;

            // If we have a variable width then assign the width value to the string size
            Width = Str.Size();
        }

        // If the string starts with the prefix and the length matches the string, remove the prefix
        else if ((Str.Size() == Width + 2) && (HasPrefix))
            Str = Str.ChopFront(2);

        // By this point, the string size and the width should match. If they don't, return false.
        if (Str.Size() != Width) return false;

        // Now we can finally check the actual string and make sure all the characters are valid hex characters.
        for (int iChr = 0; iChr < Width; iChr++)
        {
            CharType Chr = Str[iChr];
            if (!((Chr >= CHAR_LITERAL('0')) && (Chr <= CHAR_LITERAL('9'))) &&
                !((Chr >= CHAR_LITERAL('a')) && (Chr <= CHAR_LITERAL('f'))) &&
                !((Chr >= CHAR_LITERAL('A')) && (Chr <= CHAR_LITERAL('F'))))
                return false;
        }

        return true;
    }

    bool CaseInsensitiveCompare(_TStdStringView rkOther) const
    {
        if (Size() != rkOther.size())
            return false;

        for (size_t iChr = 0; iChr < Size(); iChr++)
        {
            if (CharToUpper(At(iChr)) != CharToUpper(rkOther[iChr]))
                return false;
        }

        return true;
    }

    // Hashing
    uint32 Hash32() const
    {
        return CCRC32::StaticHashData(Data(), Size() * sizeof(CharType));
    }

    uint64 Hash64() const
    {
        return CFNV1A::StaticHashData64(Data(), Size() * sizeof(CharType));
    }

    // Get Filename Components
    _TString GetFileDirectory() const
    {
        size_t EndPath = mInternalString.find_last_of(LITERAL("\\/"));
        return EndPath == _TStdString::npos ? LITERAL("") : SubString(0, EndPath + 1);
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
            size_t EndName = mInternalString.find_last_of(LITERAL("."));
            return SubString(EndPath, EndName - EndPath);
        }
    }

    _TString GetFileExtension() const
    {
        size_t EndName = mInternalString.find_last_of(LITERAL("."));
        return EndName == _TStdString::npos ? LITERAL("") : SubString(EndName + 1, Size() - EndName);
    }

    _TString GetFilePathWithoutExtension() const
    {
        size_t EndName = mInternalString.find_last_of(LITERAL("."));
        return EndName == _TStdString::npos ? *this : SubString(0, EndName);
    }

    _TString GetParentDirectoryPath(_TStdStringView rkParentDirName, bool CaseSensitive = true)
    {
        int IdxA = 0;
        int IdxB = IndexOf(LITERAL("\\/"));
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

    _TString& operator=(const _TString& rkText)
    {
        mInternalString = rkText.mInternalString;
        return *this;
    }

    _TString& operator=(_TStdStringView view)
    {
        mInternalString = view;
        return *this;
    }

    CharType& operator[](int Pos)
    {
        return mInternalString[Pos];
    }

    const CharType& operator[](int Pos) const
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
        _TString Out(Size() + 1);
        memcpy(&Out[0], mInternalString.data(), Size() * sizeof(CharType));
        memcpy(&Out[Size()], &Other, sizeof(CharType));
        return Out;
    }

    _TString operator+(const CharType* pkOther) const
    {
        uint Len = CStringLength(pkOther);

        _TString Out(Len + Size());
        memcpy(&Out[0], mInternalString.data(), Size() * sizeof(CharType));
        memcpy(&Out[Size()], pkOther, Len * sizeof(CharType));
        return Out;
    }

    _TString operator+(const _TString& rkOther) const
    {
        return (*this + rkOther.CString());
    }

    void operator+=(CharType Other)
    {
        *this = *this + Other;
    }

    void operator+=(const CharType* pkOther)
    {
        *this = *this + pkOther;
    }

    void operator+=(const _TString& rkOther)
    {
        *this = *this + rkOther;
    }

    void operator+=(_TStdStringView view)
    {
        mInternalString += view;
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
        _TString Out(rkRight.Size() + 1);
        memcpy(&Out[0], &Left, sizeof(CharType));
        memcpy(&Out[sizeof(CharType)], rkRight.CString(), rkRight.Size() * sizeof(CharType));
        return Out;
    }

    friend _TString operator+(const CharType* pkLeft, const _TString& rkRight)
    {
        uint32 Len = CStringLength(pkLeft);

        _TString Out(Len + rkRight.Size());
        memcpy(&Out[0], pkLeft, Len * sizeof(CharType));
        memcpy(&Out[Len], rkRight.CString(), rkRight.Size() * sizeof(CharType));
        return Out;
    }

    friend _TString operator+(const _TStdString& rkLeft, const _TString& rkRight)
    {
        _TString Out(rkLeft.size() + rkRight.Size());
        memcpy(&Out[0], rkLeft.data(), rkLeft.size() * sizeof(CharType));
        memcpy(&Out[rkLeft.size()], rkRight.Data(), rkRight.Size() * sizeof(CharType));
        return Out;
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
        return (mInternalString == rkOther.mInternalString);
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
        return (!(*this == rkOther));
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
        return (mInternalString < rkOther.mInternalString);
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
        return (mInternalString <= rkOther.mInternalString);
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
        return (mInternalString >= rkOther.mInternalString);
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

    // Static
    static _TString Format(const CharType *pkFmt, ...)
    {
        // Probably better to rewrite this at some point for better error handling + avoiding all the C-style syntax
        const int kBufferSize = 4096;
        CharType StringBuffer[kBufferSize];

        std::va_list Args;
        va_start(Args, pkFmt);

        if (typeid(CharType) == typeid(char))
            vsnprintf((char*) StringBuffer, kBufferSize, (char*) pkFmt, Args);
        else
            vswprintf((wchar_t*) StringBuffer, kBufferSize, (wchar_t*) pkFmt, Args);

        va_end(Args);
        return _TString(StringBuffer);
    }

    static _TString FromInt32(int32 Value, int Width = 0, int Base = 16)
    {
        std::basic_stringstream<CharType> SStream;
        SStream << std::setbase(Base) << std::setw(Width) << std::setfill(CHAR_LITERAL('0')) << Value;
        return SStream.str();
    }

    static _TString FromInt64(int64 Value, int Width = 0, int Base = 16)
    {
        std::basic_stringstream<CharType> SStream;
        SStream << std::setbase(Base) << std::setw(Width) << std::setfill(CHAR_LITERAL('0')) << Value;
        return SStream.str();
    }

    static _TString FromFloat(float Value, int MinDecimals = 1, bool Scientific = false)
    {
        _TString Out = _TString::Format(Scientific ? "%.8g" : "%f", Value);

        // Make sure we have the right number of decimals
        int DecIdx = Out.IndexOf(CHAR_LITERAL('.'));

        if (DecIdx == -1 && MinDecimals > 0)
        {
            DecIdx = Out.Size();
            Out.Append(CHAR_LITERAL('.'));
        }

        int NumZeroes = (DecIdx == -1 ? 0 : Out.Size() - (DecIdx + 1));

        // Add extra zeroes to meet the minimum decimal count
        if (NumZeroes < MinDecimals)
        {
            for (int iDec = 0; iDec < (MinDecimals - NumZeroes); iDec++)
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

    static _TString FileSizeString(int64 Size, int NumDecimals = 2)
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

        uint DecCount = Out.Size() - (Out.IndexOf(CHAR_LITERAL('.')) + 1);
        if (DecCount > NumDecimals) Out = Out.ChopBack(DecCount - NumDecimals);
        return Out + Type;
    }

    static _TString HexString(uint8 Num, int Width = 8, bool AddPrefix = true, bool Uppercase = true)
    {
        return HexString((uint32) Num, Width, AddPrefix, Uppercase);
    }

    static _TString HexString(uint16 Num, int Width = 8, bool AddPrefix = true, bool Uppercase = true)
    {
        return HexString((uint32) Num, Width, AddPrefix, Uppercase);
    }

    static _TString HexString(uint32 Num, int Width = 8, bool AddPrefix = true, bool Uppercase = true)
    {
        std::basic_stringstream<CharType> SStream;
        SStream << std::hex << std::setw(Width) << std::setfill('0') << Num;

        _TString Str = SStream.str();
        if (Uppercase) Str = Str.ToUpper();
        if (AddPrefix) Str.Prepend(LITERAL("0x"));
        return Str;
    }

    static bool CompareCStrings(const CharType* pkA, const CharType* pkB)
    {
        // Replacement for strcmp so we can compare any CharType
        while (true)
        {
            if (*pkA != *pkB) return false;
            if ((*pkA == 0) || (*pkB == 0)) return true;
            pkA++;
            pkB++;
        }
    }

    static uint32 CStringLength(const CharType* pkStr)
    {
        // Replacement for strlen so we can measure any CharType
        uint32 Out = 0;

        while (true)
        {
            if (*pkStr == 0) return Out;
            Out++;
            pkStr++;
        }
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
class TString : public TBasicString<char, std::list<TString> >
{
public:
    using BaseClass = TBasicString<char, std::list<TString> >;

    TString() {}
    TString(const BaseClass& kIn) : BaseClass(kIn) {}
    using BaseClass::BaseClass;

    void AppendCodePoint(uint32 CodePoint);
    class T16String ToUTF16() const;
    class T32String ToUTF32() const;
};

// ************ T16String ************
class T16String : public TBasicString<char16_t, std::list<T16String> >
{
public:
    using BaseClass = TBasicString<char16_t, std::list<T16String> >;

    T16String() {}
    T16String(const BaseClass& kIn) : BaseClass(kIn) {}
    using BaseClass::BaseClass;

    void AppendCodePoint(uint32 CodePoint);
    class TString ToUTF8() const;
    class T32String ToUTF32() const;
};

// ************ T32String ************
class T32String : public TBasicString<char32_t, std::list<T32String> >
{
public:
    using BaseClass = TBasicString<char32_t, std::list<T32String> >;

    T32String() {}
    T32String(const BaseClass& kIn) : BaseClass(kIn) {}
    using BaseClass::BaseClass;

    void AppendCodePoint(uint32 CodePoint);
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

#endif // TSTRING_H
