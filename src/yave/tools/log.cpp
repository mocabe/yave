//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/tools/log.hpp>
#include <spdlog/sinks/stdout_sinks.h>

namespace yave {

  std::shared_ptr<spdlog::logger> g_logger = spdlog::stdout_logger_mt("yave");

  std::shared_ptr<spdlog::logger> get_default_logger()
  {
    return g_logger;
  }

  std::shared_ptr<spdlog::logger> add_logger(const char* name)
  {
    Info(g_logger, "Added new logger {}", name);
    return spdlog::stdout_logger_mt(name);
  }

  std::shared_ptr<spdlog::logger> get_logger(const char* name)
  {
    return spdlog::get(name);
  }

}