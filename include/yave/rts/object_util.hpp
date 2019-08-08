//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/object_ptr.hpp>
#include <yave/rts/undefined.hpp>

namespace yave {

  /// \brief get **RAW** type of the object
  /// \notes return type of `Undefined` when !obj.
  /// \notes use type_of() to get actual type of terms.
  [[nodiscard]] inline object_ptr<const Type>
    get_type(const object_ptr<const Object>& obj)
  {
    if (obj)
      return _get_storage(obj).info_table()->obj_type;
    else
      return object_type<Undefined>();
  }

  /// \brief get object name in info-table.
  /// \notes return name of `Undefined` when !obj.
  [[nodiscard]] inline const char* get_name(const object_ptr<const Object>& obj)
  {
    if (obj)
      return _get_storage(obj).info_table()->obj_name;
    else
      return object_type_traits<Undefined>::name;
  }

  /// \brief get size of object.
  /// \notes return 0 when !obj.
  [[nodiscard]] inline uint64_t get_size(const object_ptr<const Object>& obj)
  {
    if (obj)
      return _get_storage(obj).info_table()->obj_size;
    else
      return 0;
  }

} // namespace yave