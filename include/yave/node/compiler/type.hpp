//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/overloaded.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>

namespace yave {

  // ------------------------------------------
  // oveloading_env

  struct overloaded_class
  {
    /// generalized class type
    object_ptr<const Type> type;
    /// instance objects
    std::vector<object_ptr<const Object>> instances;
  };

  class class_env
  {
    std::map<object_ptr<const Type>, overloaded_class, var_type_comp> m_map;

  public:
    /// register overloading info and returns new Overloaded object
    [[nodiscard]] auto add_overloading(
      const uid& id,
      const std::vector<object_ptr<const Object>>& instances)
      -> object_ptr<const Overloaded>;

    /// Finds registered overloading from ID and returns new Overloaded object
    [[nodiscard]] auto find_overloaded(const uid& id) const
      -> object_ptr<const Overloaded>;

    /// Aquire overloading info from class ID
    [[nodiscard]] auto find_overloading(
      const object_ptr<const Type>& class_id) const -> const overloaded_class*;
  };

  /// \brief dynamic type checker with overloading extension.
  /// \returns pair of type of apply tree and overloading resolved app tree.
  /// FIXME: Current implementation is very hacky and probably not theoritically
  /// correct. Would be better to implement type scheme based inference with
  /// kinds and qualified type constraints like Haskell.
  [[nodiscard]] auto type_of_overloaded(
    const object_ptr<const Object>& obj,
    class_env classes)
    -> std::pair<object_ptr<const Type>, object_ptr<const Object>>;

} // namespace yave
