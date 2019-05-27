#ifndef ASSERT_H
#define ASSERT_H

#include <cstdlib>
#include <string.h>

/** Forward decls for log functions */
namespace NLog
{
void Writef(const char* pkMsg, ...);
void Warnf(const char* pkMsg, ...);
void Errorf(const char* pkMsg, ...);
void Fatalf(const char* pkMsg, ...);
}

/**
 * Log convenience macros.
 * debugf: standard log message
 * warnf: non-critical warning message
 * errorf: triggers a debug break in debug mode
 * fatalf: triggers the application to abort
 */
#define debugf  NLog::Writef
#define warnf   NLog::Warnf
#define errorf  NLog::Errorf
#define fatalf  NLog::Fatalf

/** Returns the size of a static array */
#define ARRAY_SIZE(Array) ( sizeof(Array) / sizeof(Array[0]) )

/** Aligns the input value to be a multiple of Align. Align must be a power of 2. */
#define VAL_ALIGN(Value, Align) ( (Value + Align-1) & ~(Align-1) )

#define VAL_ALIGN_4(Value)  VAL_ALIGN(Value, 4)
#define VAL_ALIGN_8(Value)  VAL_ALIGN(Value, 8)
#define VAL_ALIGN_16(Value) VAL_ALIGN(Value, 16)
#define VAL_ALIGN_32(Value) VAL_ALIGN(Value, 32)
#define VAL_ALIGN_64(Value) VAL_ALIGN(Value, 64)

/** Returns the offset of a class member within that class */
#define MEMBER_OFFSET(TypeName, MemberName) ( (int) (long long) &((TypeName*)0)->MemberName )

/** Force a function to be inlined (disabled in debug builds) */
#if _DEBUG
    #define FORCEINLINE inline
#elif _MSC_VER
    #define FORCEINLINE __forceinline
#elif __GNUC__
    #define FORCEINLINE __attribute__((always_inline))
#endif

/**
 * Custom assert macro. ASSERT evaluates the input expression and verifies that it is true. If the
 * expression is false, an error message will be printed to the log with info on what went wrong
 * and (in debug builds) trigger a debug break. Application execution is aborted. In public
 * release builds, asserts are compiled out entirely, so neither log messages nor debug breaks will
 * occur.
 *
 * Alternatively, this file also declares an ENSURE macro, which is guaranteed always executes and
 * will never be compiled out, regardless of build configuration.
 */
#define __FILE_SHORT__ strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__

#if _DEBUG
    #define DEBUG_BREAK __debugbreak();
#else
    #define DEBUG_BREAK {}
#endif

#define ASSERT_CHECK_BEGIN(Expression) \
    { \
        if (!(Expression)) \
        {

#define ASSERT_CHECK_END \
        } \
    }

#define WRITE_FAILURE_TO_LOG(Expression) \
    fatalf("%s(%d): ASSERT FAILED: %s", __FILE_SHORT__, __LINE__, #Expression);

// ENSURE macro always executes, never gets compiled out
#define ENSURE(Expression) \
    ASSERT_CHECK_BEGIN(Expression) \
    WRITE_FAILURE_TO_LOG(Expression) \
    ASSERT_CHECK_END

#if !PUBLIC_RELEASE
    #define ASSERT(Expression) ENSURE(Expression)
#else
    #define ASSERT(Expression) {}
#endif

#endif // ASSERT_H
