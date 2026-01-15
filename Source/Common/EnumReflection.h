#ifndef CODEGEN_ENUMREFLECTION_H
#define CODEGEN_ENUMREFLECTION_H

#include <cstring>
#include <ctype.h>
#include <type_traits>
#include <map>

typedef std::map<int, const char*> CEnumNameMap;

/** Provides runtime information about enum types */
template<typename T, typename = typename std::enable_if< std::is_enum<T>::value >::type>
class TEnumReflection
{
    /** Private constructor - no instantiation */
    TEnumReflection() {}

    /** Map that provides mapping between enum constants and their names */
    static const CEnumNameMap skNameMap;

    /** Default "invalid" enum value */
    static const int skErrorValue;

public:
    /** Returns the name of the given enum value */
    inline static const char* ConvertValueToString(T InValue)
    {
        auto FindIter = skNameMap.find((int) InValue);

        if (FindIter != skNameMap.end())
        {
            return FindIter->second;
        }
        else
        {
            return nullptr;
        }
    }

    /** Returns the enum value corresponding to the given name */
    static T ConvertStringToValue(const char* InValue)
    {
        //@todo this is slow
        for (auto Iter = skNameMap.begin(); Iter != skNameMap.end(); Iter++)
        {
            if (strcmp(Iter->second, InValue) == 0)
            {
                return (T) Iter->first;
            }
        }

        return (T) skErrorValue;
    }

    /** Returns an iterator through the enum name map */
    inline static CEnumNameMap::const_iterator CreateEnumIterator()
    {
        return skNameMap.cbegin();
    }

    /**
     *  Returns the default "invalid" value of the enum.
     *  This looks for values with "Invalid" or "Unknown" in their name.
     *  If no error value is present, defaults to -1.
     */
    inline static T ErrorValue()
    {
        return (T) skErrorValue;
    }

    /** Iterator class for iterating through all possible enum values */
    class CIterator
    {
        CEnumNameMap::const_iterator mInternalIterator;

    public:
        CIterator()
        {
            mInternalIterator = skNameMap.cbegin();
        }

        inline T Value() const          { return (T) mInternalIterator->first; }
        inline const char* Name() const { return mInternalIterator->second; }
        inline operator bool() const    { return mInternalIterator != skNameMap.cend(); }
        inline CIterator& operator++()  { mInternalIterator++; return *this; }
        inline CIterator operator++(int){ CIterator RetV = *this; mInternalIterator++; return RetV; }
    };
};

/** Easier accessor function */
template<typename T>
inline const char* EnumValueName(T InEnum)
{
    return TEnumReflection<T>::ConvertValueToString(InEnum);
}

#endif
