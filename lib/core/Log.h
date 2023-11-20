//
// Created by ajaxian on 11/14/23.
//

#ifndef ORANGE_LOG_H
#define ORANGE_LOG_H

/*
Orange's logging system is based on Orange's logging system, all WL_ macros are renamed LOG_
 Orange's logging system which is based on the spdlog logging library
 (https://github.com/gabime/spdlog) and is pretty much a copy of Hazel's logging system. All HZ_ macros
 are replaced with WL_ for Orange, and Hazel namespaces have been changed to Orange.
*/

#include "LogCustomFormatters.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <map>

namespace Orange {

  class Log {
  public:
    enum class Type : uint8_t {
      Core = 0, Client = 1
    };
    enum class Level : uint8_t {
      Trace = 0, Info, Warn, Error, Fatal
    };
    struct TagDetails {
      bool Enabled = true;
      Level LevelFilter = Level::Trace;
    };

  public:
    static void Init();

    static void Shutdown();

    inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }

    inline static std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

    static bool HasTag(const std::string &tag) { return s_EnabledTags.find(tag) != s_EnabledTags.end(); }

    static std::map<std::string, TagDetails> &EnabledTags() { return s_EnabledTags; }

    template<typename... Args>
    static void PrintMessage(Log::Type type, Log::Level level, std::string_view tag, Args &&... args);

    template<typename... Args>
    static void PrintAssertMessage(Log::Type type, std::string_view prefix, Args &&... args);

  public:
    // Enum utils
    static const char *LevelToString(Level level) {
      switch (level) {
        case Level::Trace:
          return "Trace";
        case Level::Info:
          return "Info";
        case Level::Warn:
          return "Warn";
        case Level::Error:
          return "Error";
        case Level::Fatal:
          return "Fatal";
      }
      return "";
    }

    static Level LevelFromString(std::string_view string) {
      if (string == "Trace") return Level::Trace;
      if (string == "Info") return Level::Info;
      if (string == "Warn") return Level::Warn;
      if (string == "Error") return Level::Error;
      if (string == "Fatal") return Level::Fatal;

      return Level::Trace;
    }

  private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;

    inline static std::map<std::string, TagDetails> s_EnabledTags;
  };

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging
#define LOG_CORE_TRACE_TAG(tag, ...) ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Trace, tag, __VA_ARGS__)
#define LOG_CORE_INFO_TAG(tag, ...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Info, tag, __VA_ARGS__)
#define LOG_CORE_WARN_TAG(tag, ...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Warn, tag, __VA_ARGS__)
#define LOG_CORE_ERROR_TAG(tag, ...) ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Error, tag, __VA_ARGS__)
#define LOG_CORE_FATAL_TAG(tag, ...) ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Fatal, tag, __VA_ARGS__)

// Client logging
#define LOG_TRACE_TAG(tag, ...) ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Trace, tag, __VA_ARGS__)
#define LOG_INFO_TAG(tag, ...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Info, tag, __VA_ARGS__)
#define LOG_WARN_TAG(tag, ...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Warn, tag, __VA_ARGS__)
#define LOG_ERROR_TAG(tag, ...) ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Error, tag, __VA_ARGS__)
#define LOG_FATAL_TAG(tag, ...) ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Fatal, tag, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
#define LOG_CORE_TRACE(...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Trace, "", __VA_ARGS__)
#define LOG_CORE_INFO(...)   ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Info, "", __VA_ARGS__)
#define LOG_CORE_WARN(...)   ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Warn, "", __VA_ARGS__)
#define LOG_CORE_ERROR(...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Error, "", __VA_ARGS__)
#define LOG_CORE_FATAL(...)  ::Orange::Log::PrintMessage(::Orange::Log::Type::Core, ::Orange::Log::Level::Fatal, "", __VA_ARGS__)

// Client Logging
#define LOG_TRACE(...)   ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Trace, "", __VA_ARGS__)
#define LOG_INFO(...)    ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Info, "", __VA_ARGS__)
#define LOG_WARN(...)    ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Warn, "", __VA_ARGS__)
#define LOG_ERROR(...)   ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Error, "", __VA_ARGS__)
#define LOG_FATAL(...)   ::Orange::Log::PrintMessage(::Orange::Log::Type::Client, ::Orange::Log::Level::Fatal, "", __VA_ARGS__)

namespace Orange {

  template<typename... Args>
  void Log::PrintMessage(Log::Type type, Log::Level level, std::string_view tag, Args &&... args) {
    auto detail = s_EnabledTags[std::string(tag)];
    if (detail.Enabled && detail.LevelFilter <= level) {
      auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
      std::string logString = tag.empty() ? "{0}{1}" : "[{0}] {1}";
      switch (level) {
        case Level::Trace:
          //logger->trace(fmt::vformat(logString, fmt::make_format_args(tag, fmt::make_format_args(args...))));
          logger->trace(fmt::format(logString, tag, fmt::vformat(fmt::make_format_args(args...))));
          break;
        case Level::Info:
          logger->info(fmt::format(logString, tag, fmt::vformat(fmt::make_format_args(args...))));
          break;
        case Level::Warn:
          logger->warn(fmt::format(logString, tag, fmt::vformat(fmt::make_format_args(args...))));
          break;
        case Level::Error:
          logger->error(fmt::format(logString, tag, fmt::vformat(fmt::make_format_args(args...))));
          break;
        case Level::Fatal:
          logger->critical(fmt::format(logString, tag, fmt::vformat(fmt::make_format_args(args...))));
          break;
      }
    }
  }


  template<typename... Args>
  void Log::PrintAssertMessage(Log::Type type, std::string_view prefix, Args &&... args) {
    auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
    logger->error("{0}: {1}", prefix, fmt::vformat(fmt::make_format_args(args...)));
  }

  template<>
  inline void Log::PrintAssertMessage(Log::Type type, std::string_view prefix) {
    auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
    logger->error("{0}", prefix);
  }
}

#endif