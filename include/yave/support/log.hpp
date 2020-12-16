//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <spdlog/spdlog.h>

/// Macro to create local logger definition
#define YAVE_DECL_LOCAL_LOGGER(NAME)                                       \
  namespace yave {                                                         \
    namespace {                                                            \
      [[nodiscard]] auto local_logger()                                    \
      {                                                                    \
        static auto logger = yave::get_logger(#NAME);                      \
        return logger;                                                     \
      }                                                                    \
                                                                           \
      template <class... Args>                                             \
      void log_info(const char* msg, Args&&... args)                       \
      {                                                                    \
        return yave::log_info(                                             \
          local_logger(), msg, std::forward<Args>(args)...);               \
      }                                                                    \
                                                                           \
      template <class... Args>                                             \
      void log_warning(const char* msg, Args&&... args)                    \
      {                                                                    \
        return yave::log_warning(                                          \
          local_logger(), msg, std::forward<Args>(args)...);               \
      }                                                                    \
                                                                           \
      template <class... Args>                                             \
      void log_error(const char* msg, Args&&... args)                      \
      {                                                                    \
        yave::log_error(local_logger(), msg, std::forward<Args>(args)...); \
      }                                                                    \
    }                                                                      \
  }

namespace yave {

  using logger_t = std::shared_ptr<spdlog::logger>;

  /// Get logger from name or create new one
  [[nodiscard]] auto get_logger(const char* name) -> logger_t;

  /// Disable logger
  void disable_logger(const logger_t& logger);

  /// Enable logger disabled
  void enable_logger(const logger_t& logger);

  namespace detail {

    /// Log
    template <class... Args>
    void log(
      const logger_t& logger,
      spdlog::level::level_enum level,
      const char* msg,
      Args&&... args)
    {
      assert(logger);
      logger->log(level, msg, std::forward<Args>(args)...);
    }

  } // namespace detail

  /// log info
  template <class... Args>
  void log_info(const logger_t& logger, const char* msg, Args&&... args)
  {
    return detail::log(
      logger, spdlog::level::info, msg, std::forward<Args>(args)...);
  }

  /// log warning
  template <class... Args>
  void log_warning(const logger_t& logger, const char* msg, Args&&... args)
  {
    return detail::log(
      logger, spdlog::level::warn, msg, std::forward<Args>(args)...);
  }

  /// log error
  template <class... Args>
  void log_error(const logger_t& logger, const char* msg, Args&&... args)
  {
    return detail::log(
      logger, spdlog::level::err, msg, std::forward<Args>(args)...);
  }

} // namespace yave