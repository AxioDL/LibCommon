#include "CTimer.h"
#include "Log.h"
#include "Macros.h"
#include "TString.h"

#include <ctime>
#include <iostream>

namespace NLog
{

TStringList gErrorLog;
TString gLogFilename;
FILE *gpLogFile;

double gAppStartTime = CTimer::GlobalTime();

bool gInitialized = false;
TStringList gPreInitLogs;

bool InitLog(const TString& rkFilename)
{
    fopen_s(&gpLogFile, *rkFilename, "w");
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
            fopen_s(&gpLogFile, *NewFilename, "w");
            Num++;
        }

        if (!gpLogFile)
            return false;
    }

    // Print initial message to log
    time_t RawTime;
    time(&RawTime);

    tm pTimeInfo;
    localtime_s(&pTimeInfo, &RawTime);

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

void WriteInternal(EMsgType Type, const char* pkMsg, const va_list& VarArgs)
{
    char LineBuffer[512];
    double Time = CTimer::GlobalTime() - gAppStartTime;
    int Offset = sprintf(LineBuffer, "[%08.3f] ", Time);
    vsprintf(&LineBuffer[Offset], pkMsg, VarArgs);

    // Write to log file
    if (!gInitialized)
        gPreInitLogs.push_back(LineBuffer);

    else if (gpLogFile)
    {
        fprintf(gpLogFile, "%s\n", LineBuffer);
        fflush(gpLogFile);
    }

    std::cout << LineBuffer << "\n";
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
