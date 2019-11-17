#include "CTimer.h"
#include "Log.h"
#include "Macros.h"
#include "TString.h"

#include <ctime>
#include <iostream>

#ifdef WIN32
// Windows text color attributes
#include <windows.h>

#define WIN_BLACK           0
#define WIN_RED             FOREGROUND_RED
#define WIN_GREEN           FOREGROUND_GREEN
#define WIN_YELLOW          FOREGROUND_RED | FOREGROUND_GREEN
#define WIN_BLUE            FOREGROUND_BLUE
#define WIN_MAGENTA         FOREGROUND_RED | FOREGROUND_BLUE
#define WIN_CYAN            FOREGROUND_GREEN | FOREGROUND_BLUE
#define WIN_WHITE           FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN
#define WIN_BOLD            FOREGROUND_INTENSITY
#define WIN_RESET           WIN_WHITE

WORD GetColorCode(NLog::EMsgType Type)
{
    switch (Type)
    {
    case NLog::EMsgType::Warning:   return WIN_BOLD | WIN_YELLOW;
    case NLog::EMsgType::Error:     return WIN_BOLD | WIN_RED;
    case NLog::EMsgType::Fatal:     return WIN_RED;
    default:                        return WIN_RESET;
    }
}

#else

// ANSI color codes for Unix
#define ANSI_BLACK          "\x1b[30m"
#define ANSI_RED            "\x1b[31m"
#define ANSI_GREEN          "\x1b[32m"
#define ANSI_YELLOW         "\x1b[33m"
#define ANSI_BLUE           "\x1b[34m"
#define ANSI_MAGENTA        "\x1b[35m"
#define ANSI_CYAN           "\x1b[36m"
#define ANSI_WHITE          "\x1b[37m"
#define ANSI_BOLD_BLACK     "\x1b[1;30m"
#define ANSI_BOLD_RED       "\x1b[1;31m"
#define ANSI_BOLD_GREEN     "\x1b[1;32m"
#define ANSI_BOLD_YELLOW    "\x1b[1;33m"
#define ANSI_BOLD_BLUE      "\x1b[1;34m"
#define ANSI_BOLD_MAGENTA   "\x1b[1;35m"
#define ANSI_BOLD_CYAN      "\x1b[1;36m"
#define ANSI_BOLD_WHITE     "\x1b[1;37m"
#define ANSI_RESET          "\x1b[0m"

const char* GetColorCode(NLog::EMsgType Type)
{
    switch (Type)
    {
    case NLog::EMsgType::Warning:   return ANSI_BOLD_YELLOW;
    case NLog::EMsgType::Error:     return ANSI_BOLD_RED;
    case NLog::EMsgType::Fatal:     return ANSI_RED;
    default:                        return ANSI_RESET;
    }
}

#endif

namespace NLog
{

TStringList gErrorLog;
TString gLogFilename;
FILE *gpLogFile;

double gAppStartTime = CTimer::GlobalTime();

bool gInitialized = false;
TStringList gPreInitLogs;

#ifdef _WIN32
static int log_fopen(FILE** pFile, const char *filename, const char *mode)
{
    return fopen_s(pFile, filename, mode);
}
static int log_localtime(struct tm* const tmDest, time_t const* const sourceTime)
{
    return localtime_s(tmDest, sourceTime);
}
#else
static int log_fopen(FILE** pFile, const char *filename, const char *mode)
{
    *pFile = fopen(filename, mode);
    return *pFile == nullptr;
}
static int log_localtime(struct tm* const tmDest, time_t const* const sourceTime)
{
    return localtime_r(sourceTime, tmDest) == nullptr;
}
#endif

bool InitLog(const TString& rkFilename)
{
    log_fopen(&gpLogFile, *rkFilename, "w");
    gLogFilename = rkFilename;

    if (!gpLogFile)
    {
        TString FileName = rkFilename.GetFileName(false);
        TString Extension = rkFilename.GetFileExtension();
        int Num = 0;

        while (!gpLogFile)
        {
            if (Num > 999) break;
            TString NewFilename = FileName + "_" + TString::FromInt32(Num, 0, 10) + "." + Extension;
            log_fopen(&gpLogFile, *NewFilename, "w");
            Num++;
        }

        if (!gpLogFile)
            return false;
    }

    // Print initial message to log
    time_t RawTime;
    time(&RawTime);

    tm pTimeInfo;
    log_localtime(&pTimeInfo, &RawTime);

    char Buffer[80];
    strftime(Buffer, 80, "%m/%d/%y %H:%M:%S", &pTimeInfo);

    fprintf(gpLogFile, "Opened log file at %s\n", Buffer);
    fflush(gpLogFile);

#ifdef APP_FULL_NAME
    // Print app name and version
    fprintf(gpLogFile, APP_FULL_NAME"\n");
#endif
    gInitialized = true;

    // Print any messages that were attempted before we initialized
    if (!gPreInitLogs.empty())
    {
        for (auto it = gPreInitLogs.begin(); it != gPreInitLogs.end(); it++)
            Writef(**it);

        gPreInitLogs.clear();
    }

    return true;
}

void WriteInternal(EMsgType Type, const char* pkMsg, va_list VarArgs)
{
    // Format current time to a string
    char TimeBuffer[16];
    double Time = CTimer::GlobalTime() - gAppStartTime;
    snprintf(TimeBuffer, 16, "[%08.3f]", Time);

    // Format message with varargs
    char MsgBuffer[512];
    vsnprintf(MsgBuffer, 512, pkMsg, VarArgs);

    // Write to log file
    if (!gInitialized)
    {
        gPreInitLogs.push_back(TimeBuffer);
    }
    else if (gpLogFile)
    {
        fprintf(gpLogFile, "%s %s\n", TimeBuffer, MsgBuffer);
        fflush(gpLogFile);
    }

    // Write to stdout
#ifdef WIN32
    static HANDLE sStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(sStdOutHandle, WIN_BOLD | WIN_CYAN);
    printf("%s ", TimeBuffer);
    SetConsoleTextAttribute(sStdOutHandle, GetColorCode(Type));
    printf(MsgBuffer);
    SetConsoleTextAttribute(sStdOutHandle, WIN_RESET);
    printf("\n");
    // Also output using MS' debugger API
    char DebugBuffer[530];
    snprintf(DebugBuffer, 530, "%s %s\n", TimeBuffer, MsgBuffer);
    OutputDebugStringA(DebugBuffer);
#else
    printf(ANSI_BOLD_CYAN "%s %s%s" ANSI_RESET "\n",
           TimeBuffer, GetColorCode(Type), MsgBuffer);
#endif
}

#define DEFINE_LOG_FUNC(MsgType) \
    va_list VarArgs; \
    va_start(VarArgs, pkMsg); \
    WriteInternal(MsgType, pkMsg, VarArgs); \
    va_end(VarArgs); \

void Writef(const char* pkMsg, ...)
{
    DEFINE_LOG_FUNC(EMsgType::Standard);
}

void Warnf(const char* pkMsg, ...)
{
    DEFINE_LOG_FUNC(EMsgType::Warning);
}

void Errorf(const char* pkMsg, ...)
{
    DEFINE_LOG_FUNC(EMsgType::Error);
    DEBUG_BREAK;
}

void Fatalf(const char* pkMsg, ...)
{
    DEFINE_LOG_FUNC(EMsgType::Fatal);
    DEBUG_BREAK;
    abort();
}

const TStringList& GetErrorLog()
{
    return gErrorLog;
}

void ClearErrorLog()
{
    gErrorLog.clear();
}

}
