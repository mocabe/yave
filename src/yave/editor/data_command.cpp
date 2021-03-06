//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/data_command.hpp>

namespace yave::editor {

  auto get_data_command_memory_resource() noexcept -> std::pmr::memory_resource*
  {
    return std::pmr::get_default_resource();
  }
}