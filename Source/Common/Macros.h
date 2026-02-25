#ifndef AXIO_ASSERT_H
#define AXIO_ASSERT_H

#include <cstdlib>
#include <cstring>
#include <Common/Log.h>

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
    NLog::Fatal("{}({}): ASSERT FAILED: {}", __FILE_SHORT__, __LINE__, #Expression);

// ENSURE macro always executes, never gets compiled out
#define ENSURE(Expression)           \
    ASSERT_CHECK_BEGIN(Expression)   \
    WRITE_FAILURE_TO_LOG(Expression) \
    DEBUG_BREAK                      \
    ASSERT_CHECK_END

#ifdef PUBLIC_RELEASE
    #define ASSERT(Expression) {}
#else
    #define ASSERT(Expression) ENSURE(Expression)
#endif

#endif // AXIO_ASSERT_H
