//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/keyframe.hpp>
#include <yave/node/core/node_graph.hpp>

#include <yave/support/error.hpp>

namespace yave {
  void desugar_KeyframeInt(node_graph& g, const node_handle& n, error_list&);
  void desugar_KeyframeFloat(node_graph& g, const node_handle& n, error_list&);
  void desugar_KeyframeBool(node_graph& g, const node_handle& n, error_list&);
}