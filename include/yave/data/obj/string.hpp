//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/string/string.hpp>

#include <string>
#include <cstring>
#include <string>

namespace yave {

  /// UTF-8 String object.
  /// Does not handle anything about other encodings. User must ensure
  /// input byte sequence is null(`0x00`)-terminated UTF-8 string.
  using String = Box<yave::string>;

  namespace literals {

    /// String object literal
    [[nodiscard]] inline object_ptr<String>
      operator"" _S(const char* str, size_t)
    {
      return make_object<String>(str);
    }

  } // namespace literals

} // namespace yave

// String
YAVE_DECL_TYPE(yave::String, "1198939c-c273-4875-a229-245abad7ef04");
