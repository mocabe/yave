//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/node/core/node_handle.hpp>

#include <string>

namespace yave {

  /// Scope of layer resources
  enum class layer_resource_scope : uint32_t
  {
    Inherit = 1, ///< Resouce which can also be accessed from sublayers.
    Private = 2, ///< Resouce which is private in the layer
    Global  = 3, ///< Global resource (do we really need this?)
  };

  /// to_string
  constexpr const char* to_string(layer_resource_scope scope) {
    switch(scope) {
      case layer_resource_scope::Inherit:
        return "layer_resource_scope::Inherit";
      case layer_resource_scope::Private:
        return "layer_resource_scope::Private";
      case layer_resource_scope::Global:
        return "layer_resource_scope::Global";
    }
    return "(unrecognized layer scope)";
  }

  /// Layer resource
  struct layer_resource
  {
    /// name of resource (optional)
    std::string name;
    /// handle to resource
    node_handle handle;
    /// scope of this resource
    layer_resource_scope scope;
  };

}