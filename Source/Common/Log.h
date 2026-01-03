#ifndef LOG_H
#define LOG_H

#include <Common/TString.h>
#include <spdlog/spdlog.h>

/** Application logging functionality */
namespace NLog
{

bool InitLog(const TString& rkFilename);

inline auto Get()
{
    return spdlog::get("axio");
}

template <typename... Args>
void Debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Get()->debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void Warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Get()->warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void Error(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Get()->error(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void Fatal(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Get()->critical(fmt, std::forward<Args>(args)...);
}

} // namespace NLog

#endif // LOG_H

