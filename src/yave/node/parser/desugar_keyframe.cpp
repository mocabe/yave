//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/desugar_keyframe.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/obj/constructor.hpp>

namespace yave {

  template <class KeyframeT, class KeyframeTValueExtractor>
  void desugar_KeyframeT(node_graph& g, const node_handle& n, error_list& e)
  {
    assert(g.get_info(n) == get_node_info<KeyframeT>());

    auto prim = g.get_primitive(n);
    assert(prim);

    auto kf_info = get_node_info<KeyframeT>();
    auto ex_info = get_node_info<KeyframeTValueExtractor>();

    auto keyframe  = g.add(kf_info, *prim);
    auto extractor = g.add(ex_info);

    assert(prim);
    assert(extractor);

    // kf -> ex
    {
      auto c = g.connect(
        keyframe,
        kf_info.output_sockets()[0],
        extractor,
        ex_info.input_sockets()[0]);

      if (!c) {
        e.push_back(make_error<parse_error::unexpected_error>(
          "Faled to connect Keyframe primitive to value extractor"));
        throw std::runtime_error("Failed to connect nodes");
      }
    }

    auto inputs = g.input_connections(n);

    // report missing 
    if (inputs.empty()) {
      e.push_back(make_error<parse_error::no_sufficient_input>(n));
      g.remove(keyframe);
      g.remove(extractor);
      return;
    }

    assert(inputs.size() == 1);

    // input -> ex[1]
    for (auto&& ic : inputs) {
      auto ic_info = g.get_info(ic);
      assert(ic_info);
      auto c = g.connect(
        g.get_info(ic)->src_node(),
        g.get_info(ic)->src_socket(),
        extractor,
        ex_info.input_sockets()[1]);

      if (!c) {
        e.push_back(make_error<parse_error::unexpected_error>(
          "Failed to create connection to Keyframe value extractor"));
        throw std::runtime_error("Failed to connect nodes");
      }
    }

    // ex -> outputs
    for (auto&& oc : g.output_connections(n)) {
      auto oc_info = g.get_info(oc);
      assert(oc_info);

      auto c = g.connect(
        extractor,
        ex_info.output_sockets()[0],
        oc_info->dst_node(),
        oc_info->dst_socket());

      if (!c) {
        e.push_back(make_error<parse_error::unexpected_error>(
          "Failed to create connection from Keyframe value extractor"));
        throw std::runtime_error("Failed to connect nodes");
      }
    }

    // remove original node
    g.remove(n);
  }

  void desugar_KeyframeInt(node_graph& g, const node_handle& n, error_list& e)
  {
    return desugar_KeyframeT<
      node::KeyframeInt,
      node::KeyframeIntValueExtractor>(g, n, e);
  }

  void desugar_KeyframeFloat(node_graph& g, const node_handle& n, error_list& e)
  {
    return desugar_KeyframeT<
      node::KeyframeFloat,
      node::KeyframeFloatValueExtractor>(g, n, e);
  }

  void desugar_KeyframeBool(node_graph& g, const node_handle& n, error_list& e)
  {
    return desugar_KeyframeT<
      node::KeyframeBool,
      node::KeyframeBoolValueExtractor>(g, n, e);
  }
} // namespace yave