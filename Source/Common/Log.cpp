#include "Common/Log.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _WIN32
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace NLog
{
static bool gInitialized = false;

bool InitLog(const std::string& filename)
{
    if (gInitialized)
        return true;

    try
    {
        std::vector<spdlog::sink_ptr> sinks{
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true),
#ifdef _WIN32
            std::make_shared<spdlog::sinks::msvc_sink_mt>(true)
#endif
        };
        auto logger = std::make_shared<spdlog::logger>("axio", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::debug);
        logger->info("Opened log file at: {:%m/%d/%y %H:%M:%S}", std::chrono::system_clock::now());
#ifdef APP_FULL_NAME
        logger->info("{}", APP_FULL_NAME "\n");
#endif
        logger->flush();
        spdlog::register_logger(logger);

        gInitialized = true;
        return true;
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Unable to initialize logger: " << ex.what() << '\n';
        return false;
    }
}

}
