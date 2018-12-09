#ifndef LOG_H
#define LOG_H

#include <Common/TString.h>

/** Application logging functionality */
//@todo: make this object-oriented and allow multiple handlers for log messages
namespace NLog
{

enum class EMsgType
{
    Standard,
    Warning,
    Error,
    Fatal
};

bool InitLog(const TString& rkFilename);
void Writef(const char* pkMsg, ...);
void Warnf(const char* pkMsg, ...);
void Errorf(const char* pkMsg, ...);
void Fatalf(const char* pkMsg, ...);
const TStringList& GetErrorLog();
void ClearErrorLog();

}

#endif // LOG_H

