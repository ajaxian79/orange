#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <filesystem>

namespace Orange {

  std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
  std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

  void Log::Init() {
    // NOTE(Yan): disabled for now because logging macros need some work

    return;
    // Create "logs" directory if doesn't exist
    std::string logsDirectory = "logs";
    if (!std::filesystem::exists(logsDirectory))
      std::filesystem::create_directories(logsDirectory);

    std::vector<spdlog::sink_ptr> orangeSinks =
        {
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Orange.log", true),
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        };

    std::vector<spdlog::sink_ptr> appSinks =
        {
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/APP.log", true),

            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()

        };

    orangeSinks[0]->set_pattern("[%T] [%l] %n: %v");
    appSinks[0]->set_pattern("[%T] [%l] %n: %v");


    orangeSinks[1]->set_pattern("%^[%T] %n: %v%$");
    appSinks[1]->set_pattern("%^[%T] %n: %v%$");


    s_CoreLogger = std::make_shared<spdlog::logger>("HAZEL", orangeSinks.begin(), orangeSinks.end());
    s_CoreLogger->set_level(spdlog::level::trace);

    s_ClientLogger = std::make_shared<spdlog::logger>("APP", appSinks.begin(), appSinks.end());
    s_ClientLogger->set_level(spdlog::level::trace);
  }

  void Log::Shutdown() {
    s_ClientLogger.reset();
    s_CoreLogger.reset();
    spdlog::drop_all();
  }

}
