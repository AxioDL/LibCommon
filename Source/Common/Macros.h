#ifndef ASSERT_H
#define ASSERT_H

#include "Log.h"
#include "TString.h"
#include <cstdlib>
#include <string.h>

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

/** Aligns the input value to be a multiple of Align. Align must be a power of 2. */
#define ALIGN(Value, Align) ( (Value + Align-1) & ~(Align-1) )

#define ALIGN_4(Value)  ALIGN(Value, 4)
#define ALIGN_8(Value)  ALIGN(Value, 8)
#define ALIGN_16(Value) ALIGN(Value, 16)
#define ALIGN_32(Value) ALIGN(Value, 32)
#define ALIGN_64(Value) ALIGN(Value, 64)

/** Returns the offset of a class member within that class */
#define MEMBER_OFFSET(TypeName, MemberName) ( (int) (long long) &((TypeName*)0)->MemberName )

/**
 * ASSERT evaluates the input expression and verifies that it is true. If the expression is false, an error
 * message will be printed to the log with info on what went wrong and (in debug builds) trigger a debug
 * break. Application execution is not aborted, but if an assert fails then there is usually a decent chance
 * of the application crashing regardless.
 *
 * Note that in public release builds, asserts are compiled out entirely, so neither log messages nor debug breaks
 * will occur.
 */
#define __FILE_SHORT__ strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__

#if _DEBUG
    #define DEBUG_BREAK __debugbreak();
#else
    #define DEBUG_BREAK {}
#endif

#if 1

    // Asserts Enabled
    #define ASSERT_CHECK_BEGIN(Expression) \
        { \
            if (!(Expression)) \
            {

    #define ASSERT_CHECK_END \
            } \
        }

    #define WRITE_FAILURE_TO_LOG(Expression) \
        errorf("%s(%d): ASSERT FAILED: %s", __FILE_SHORT__, __LINE__, #Expression);

    #define ASSERT(Expression) \
        ASSERT_CHECK_BEGIN(Expression) \
        WRITE_FAILURE_TO_LOG(Expression) \
        DEBUG_BREAK; \
        ASSERT_CHECK_END

#else

    // Asserts Disabled
    #define ASSERT(Expression) {}

#endif

#endif // ASSERT_H
