//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/log.hpp>

#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace yave {

  auto get_logger(const char* name) -> logger_t
  {
    if (auto logger = spdlog::get(name)) {
      return logger;
    }
    return spdlog::stdout_logger_mt(name);
  }

  void disable_logger(const logger_t& logger)
  {
    logger->set_level(spdlog::level::off);
  }

  void enable_logger(const logger_t& logger)
  {
    logger->set_level(spdlog::level::info);
  }

} // namespace yave