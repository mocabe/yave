//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/errors.hpp>

#include <fmt/format.h>

namespace yave {

  // TODO: formatted error messages.

  std::string parse_errors::no_valid_overloading::message() const
  {
    return "No Valid Overloading";
  }

  std::string parse_errors::ambiguous_overloading::message() const
  {
    return "Ambigusou overloading";
  }

  std::string parse_errors::type_missmatch::message() const
  {
    return "Type missmatch";
  }
}