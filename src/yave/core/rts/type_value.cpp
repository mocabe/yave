//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/rts/type_value.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fmt/format.h>

namespace yave {

  std::string value_type::to_string() const
  {
    boost::uuids::uuid id {};
    std::memcpy(id.data, data->data(), sizeof(data));
    return fmt::format("{}({})", name, boost::uuids::to_string(id));
  }
}