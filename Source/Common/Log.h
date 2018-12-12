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
const TStringList& GetErrorLog();
void ClearErrorLog();

// Note: If you change these, also update the forward decls in Macros.h
void Writef(const char* pkMsg, ...);
void Warnf(const char* pkMsg, ...);
void Errorf(const char* pkMsg, ...);
void Fatalf(const char* pkMsg, ...);

}

#endif // LOG_H

