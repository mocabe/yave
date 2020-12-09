//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/serialize.hpp>
#include <yave/support/overloaded.hpp>

#include <map>
#include <variant>
#include <string>

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>

#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/optional.hpp>

#include <range/v3/view.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace yave {

  namespace {

    namespace rs = ranges;
    namespace rv = ranges::views;

    template <class Handle>
    struct hid
    {
      uint64_t id = {0};

      hid()           = default;
      hid(const hid&) = default;
      hid& operator=(const hid&) = default;

      hid(const Handle& h)
        : id {h.id().data}
      {
      }

      auto operator<=>(const hid&) const = default;

      template <class Archive>
      auto save_minimal(const Archive&) const
      {
        return id;
      }

      template <class Archive>
      void load_minimal(Archive&, const uint64_t& value)
      {
        id = value;
      }
    };

    using nid = hid<node_handle>;
    using sid = hid<socket_handle>;
    using cid = hid<connection_handle>;

    struct ptype
    {
      std::string uuid;
      std::string name;

      template <class Archive>
      void serialize(Archive& ar)
      {
        ar(CEREAL_NVP(uuid), CEREAL_NVP(name));
      }
    };

    using pdata = std::variant<int64_t, double, std::string, bool>;

    struct pnode
    {
      std::string name;
      std::optional<pdata> data;
      std::optional<ptype> type;
      std::vector<pnode> children;

      template <class Archive>
      void serialize(Archive& ar)
      {
        ar(
          CEREAL_NVP(name),
          CEREAL_NVP(data),
          CEREAL_NVP(type),
          CEREAL_NVP(children));
      }
    };

    auto make_pnode(std::string n, pdata d)
    {
      return pnode {.name = n, .data = d, .type = {}, .children = {}};
    }

    auto make_pnode(std::string n, ptype t, std::vector<pnode> cs)
    {
      return pnode {.name = n, .data = {}, .type = t, .children = cs};
    }

    enum class ntype
    {
      group,
      group_input,
      group_output,
      function,
      macro,
    };

    enum class ctype
    {
      call,
      definition,
    };

    struct ndata
    {
      nid id;
      nid parent;
      std::string name;
      std::string defpath;
      ntype ntp;
      ctype ctp;
      std::vector<sid> iss;
      std::vector<sid> oss;
      std::vector<std::pair<std::string, pnode>> props;

      template <class Archive>
      void serialize(Archive& ar)
      {
        ar(
          CEREAL_NVP(id),
          CEREAL_NVP(parent),
          CEREAL_NVP(name),
          CEREAL_NVP(defpath),
          CEREAL_NVP(ntp),
          CEREAL_NVP(ctp),
          CEREAL_NVP(iss),
          CEREAL_NVP(oss),
          CEREAL_NVP(props));
      }
    };

    struct sdata
    {
      sid id;
      nid parent;
      std::string name;
      std::vector<std::pair<std::string, pnode>> props;

      template <class Archive>
      void serialize(Archive& ar)
      {
        ar(
          CEREAL_NVP(id),
          CEREAL_NVP(parent),
          CEREAL_NVP(name),
          CEREAL_NVP(props));
      }
    };

    struct cdata
    {
      cid id;
      sid src;
      sid dst;

      template <class Archive>
      void serialize(Archive& ar)
      {
        ar(CEREAL_NVP(id), CEREAL_NVP(src), CEREAL_NVP(dst));
      }
    };

    struct ngdata
    {
      nid root;
      std::vector<ndata> ns;
      std::vector<sdata> ss;
      std::vector<cdata> cs;

      template <class Archive>
      void serialize(Archive& ar, const uint32_t)
      {
        ar(CEREAL_NVP(root), CEREAL_NVP(ns), CEREAL_NVP(ss), CEREAL_NVP(cs));
      }
    };

    auto save_uuid(const std::array<char, 16>& bytes)
    {
      auto uuid = boost::uuids::uuid();
      std::memcpy(uuid.data, bytes.data(), uuid.size());
      return boost::lexical_cast<std::string>(uuid);
    }

    auto load_uuid(const std::string& id)
    {
      auto uuid  = boost::lexical_cast<boost::uuids::uuid>(id);
      auto bytes = std::array<char, 16> {};
      std::memcpy(bytes.data(), uuid.data, bytes.size());
      return bytes;
    }

    // convert property tree to serializable message
    auto save_property_tree(const object_ptr<PropertyTreeNode>& p) -> pnode
    {
      if (p->is_value()) {

        auto val = p->get_value_untyped();

        pdata data;
        {
          /**/ if (auto i = value_cast_if<Int>(val))
            data = *i;
          else if (auto f = value_cast_if<Float>(val))
            data = *f;
          else if (auto s = value_cast_if<String>(val))
            data = *s;
          else if (auto b = value_cast_if<Bool>(val))
            data = *b;
          else
            unreachable();
        }

        return make_pnode(p->name(), data);
      }

      ptype type;
      {
        auto tp = p->type();
        if (auto tcon = get_if<tcon_type>(tp.value())) {
          assert(tcon->kind == nullptr); // should be 0-ary
          type.uuid = save_uuid(tcon->id);
          type.name = tcon->name;
        } else
          unreachable();
      }

      auto&& children = p->children();

      auto cs = children //
                | rv::transform([](auto&& c) { return save_property_tree(c); })
                | rs::to_vector;

      return make_pnode(p->name(), type, std::move(cs));
    }

    // reconstruct property tree form serialized data
    auto load_property_tree(const pnode& p) -> object_ptr<PropertyTreeNode>
    {
      // value node
      if (p.children.empty()) {
        return std::visit(
          overloaded {
            [&](std::string s) {
              // need std::string -> data::string conversion
              return make_object<PropertyTreeNode>(
                p.name, make_object<String>(s));
            },
            [&]<class T>(T x) {
              // for int64_t, double, bool
              return make_object<PropertyTreeNode>(
                p.name, make_object<Box<T>>(x));
            }},
          p.data.value());
      }

      // FIXME: do not use implementation detail of rts!
      auto tp = make_object<Type>(tcon_type {
        load_uuid(p.type.value().uuid),
        detail::kind_address<kstar>(),
        "(deserialized value type)"});

      auto cs = p.children //
                | rv::transform([](auto&& c) { return load_property_tree(c); })
                | rs::to_vector;

      return make_object<PropertyTreeNode>(
        p.name, std::move(tp), std::move(cs));
    }

    [[nodiscard]] auto save_node_data(
      const structured_node_graph& ng,
      const node_handle& root) -> ngdata
    {
      auto ret = ngdata();
      ret.root = nid(root);

      if (!ng.exists(root))
        throw std::runtime_error("Invalid root specified on serialization");

      std::vector<uid> marks;

      // mark node as processed
      auto mark = [&](const node_handle& n) {
        auto lb = std::lower_bound(marks.begin(), marks.end(), n.id());
        if (lb != marks.end())
          assert(*lb != n.id());
        marks.insert(lb, n.id());
      };

      // is node marked?
      auto marked = [&](const node_handle& n) {
        auto lb = std::lower_bound(marks.begin(), marks.end(), n.id());
        return lb != marks.end() && *lb == n.id();
      };

      // convert property map
      auto cvt_props = [&](auto&& h) {
        auto r = std::vector<std::pair<std::string, pnode>>();
        for (auto&& [name, obj] : ng.get_properties(h)) {
          r.emplace_back(name, save_property_tree(obj));
        }
        return r;
      };

      // convert node to ndata
      auto cvt_n = [&](auto& n, auto& info) {
        auto nd   = ndata();
        nd.id     = n;
        nd.parent = ng.get_parent_group(n);
        nd.name   = ng.get_name(n).value();

        // io do not have definition
        if (auto d = ng.get_definition(n)) {
          nd.defpath = ng.get_path(d).value();
        } else {
          nd.defpath = ng.get_path(n).value();
        }

        nd.ntp = [&] {
          switch (info->type()) {
            case structured_node_type::function:
              return ntype::function;
            case structured_node_type::group:
              return ntype::group;
            case structured_node_type::macro:
              return ntype::macro;
            case structured_node_type::group_input:
              return ntype::group_input;
            case structured_node_type::group_output:
              return ntype::group_output;
          }
          unreachable();
        }();

        nd.ctp = [&] {
          switch (info->call_type()) {
            case structured_call_type::call:
              return ctype::call;
            case structured_call_type::definition:
              return ctype::definition;
          }
          unreachable();
        }();

        for (auto&& s : ng.input_sockets(n))
          nd.iss.emplace_back(s);

        for (auto&& s : ng.output_sockets(n))
          nd.oss.emplace_back(s);

        nd.props = cvt_props(n);

        return nd;
      };

      // convert socket to sdata
      auto cvt_s = [&](auto& s, auto& info) {
        auto sd   = sdata();
        sd.id     = s;
        sd.parent = info->node();
        sd.name   = info->name();
        sd.props  = cvt_props(s);
        return sd;
      };

      // convert connection to cdata
      auto cvt_c = [&](auto& c, auto& info) {
        auto cd = cdata();
        cd.id   = c;
        cd.src  = info->src_socket();
        cd.dst  = info->dst_socket();
        return cd;
      };

      auto rec_n = [&](auto&& self, const auto& n) {
        // already visited
        if (marked(n))
          return;

        // visit
        mark(n);

        auto ninfo = ng.get_info(n);
        ret.ns.push_back(cvt_n(n, ninfo));

        for (auto&& s : ninfo->input_sockets()) {
          auto sinfo = ng.get_info(s);
          ret.ss.push_back(cvt_s(s, sinfo));
        }

        for (auto&& s : ninfo->output_sockets()) {
          auto sinfo = ng.get_info(s);
          ret.ss.push_back(cvt_s(s, sinfo));
        }

        for (auto&& c : ng.input_connections(n)) {
          auto cinfo = ng.get_info(c);
          ret.cs.push_back(cvt_c(c, cinfo));
        }

        // traverse group
        if (ng.is_group(n) && ng.is_definition(n)) {
          for (auto&& c : ng.get_group_nodes(n))
            self(c);
        }
      };

      auto rec = fix_lambda(rec_n);
      rec(root);
      return ret;
    }

    [[nodiscard]] auto load_node_data(
      ngdata& nd,
      structured_node_graph& ng,
      node_handle& root)
    {
      // sort id arrays
      auto cmp = [](auto&& l, auto&& r) { return l.id < r.id; };
      std::sort(nd.ns.begin(), nd.ns.end(), cmp);
      std::sort(nd.ss.begin(), nd.ss.end(), cmp);
      std::sort(nd.cs.begin(), nd.cs.end(), cmp);

      auto find = [&](auto&& id, auto&& ids) -> auto&
      {
        auto it =
          std::lower_bound(ids.begin(), ids.end(), id, [](auto&& l, auto&& r) {
            return l.id < r;
          });
        assert(it != ids.end() && it->id == id);
        return *it;
      };

      // mapping
      auto smap = std::map<sid, socket_handle>();
      auto nmap = std::map<nid, node_handle>();

      // pass 1: load definitions
      auto proc_def = [&](const ndata& n) {
        if (n.ctp != ctype::definition)
          return;

        switch (n.ntp) {
          case ntype::function:
            // function definition should not be serialized
            unreachable();
            break;
          case ntype::macro:
            // macro definition should not be serialized
            unreachable();
            break;
          case ntype::group: {

            auto h = [&] {
              // already created
              if (auto hs = ng.search_path(n.defpath); !hs.empty()) {
                assert(hs.size() == 1);
                return hs[0];
              }
              // create as empty group
              return ng.create_group(n.defpath, {}, {});
            }();

            assert(h);
            nmap.emplace(n.id, h);

            assert(ng.input_sockets(h).empty() && ng.output_sockets(h).empty());

            for (size_t i = 0; i < n.iss.size(); ++i) {
              auto s = ng.add_input_socket(h, find(n.iss[i], nd.ss).name);
              assert(s);
              smap.emplace(n.iss[i], s);
            }

            for (size_t i = 0; i < n.oss.size(); ++i) {
              auto s = ng.add_output_socket(h, find(n.oss[i], nd.ss).name);
              assert(s);
              smap.emplace(n.oss[i], s);
            }

            break;
          }
          case ntype::group_input:
            unreachable();
            break;
          case ntype::group_output:
            unreachable();
            break;
        }
      };

      // pass2: load calls
      auto proc_call = [&](const ndata& n) {
        if (n.ctp != ctype::call)
          return;

        switch (n.ntp) {
          case ntype::function:
          case ntype::group: {

            auto h = ng.create_copy(
              nmap.at(n.parent), ng.search_path(n.defpath).at(0));

            assert(h);
            nmap.emplace(n.id, h);

            assert(ng.is_function(h) || ng.is_group(h));

            auto iss = ng.input_sockets(h);
            assert(n.iss.size() == iss.size());
            for (size_t i = 0; i < n.iss.size(); ++i) {
              smap.emplace(n.iss[i], iss[i]);
            }

            auto oss = ng.output_sockets(h);
            assert(n.oss.size() == oss.size());
            for (size_t i = 0; i < n.oss.size(); ++i) {
              smap.emplace(n.oss[i], oss[i]);
            }

            break;
          }
          case ntype::macro: {

            auto h = ng.create_copy(
              nmap.at(n.parent), ng.search_path(n.defpath).at(0));

            assert(h);
            nmap.emplace(n.id, h);

            assert(ng.is_macro(h));

            for (auto&& s : ng.input_sockets(h))
              ng.remove_socket(s);

            for (auto&& s : ng.output_sockets(h))
              ng.remove_socket(s);

            for (size_t i = 0; i < n.iss.size(); ++i) {
              auto s = ng.add_input_socket(h, find(n.iss[i], nd.ss).name);
              assert(s);
              smap.emplace(n.iss[i], s);
            }

            for (size_t i = 0; i < n.oss.size(); ++i) {
              auto s = ng.add_output_socket(h, find(n.oss[i], nd.ss).name);
              assert(s);
              smap.emplace(n.oss[i], s);
            }

            break;
          }
          case ntype::group_input: {
            auto h = ng.get_group_input(nmap.at(n.parent));
            nmap.emplace(n.id, h);

            ng.set_name(h, n.name);

            auto oss = ng.output_sockets(h);
            assert(n.oss.size() == oss.size());
            for (size_t i = 0; i < n.oss.size(); ++i) {
              smap.emplace(n.oss[i], oss[i]);
            }
            break;
          }
          case ntype::group_output: {
            auto h = ng.get_group_output(nmap.at(n.parent));
            nmap.emplace(n.id, h);

            ng.set_name(h, n.name);

            auto iss = ng.input_sockets(h);
            assert(n.iss.size() == iss.size());
            for (size_t i = 0; i < n.iss.size(); ++i) {
              smap.emplace(n.iss[i], iss[i]);
            }
            break;
          }
        }
      };

      auto proc_conn = [&](const cdata& c) {
        auto src = smap.at(c.src);
        auto dst = smap.at(c.dst);
        ng.connect(src, dst);
      };

      for (auto&& n : nd.ns)
        proc_def(n);

      for (auto&& n : nd.ns)
        proc_call(n);

      for(auto&& c : nd.cs)
        proc_conn(c);

      // load node properties
      for (auto&& [id, h] : nmap) {
        for (auto&& [name, prop] : find(id, nd.ns).props) {
          ng.set_property(h, name, load_property_tree(prop));
        }
      }

      // load socket properties
      for (auto&& [id, h] : smap) {
        for (auto&& [name, prop] : find(id, nd.ss).props) {
          ng.set_property(h, name, load_property_tree(prop));
        }
      }

      // set root
      root = nmap.at(nd.root);
    }

  } // namespace

  template <class Archive>
  void save_user_node_graph(
    Archive& ar,
    const node_handle& root,
    const structured_node_graph& ng)
  {
    // convert to message
    auto node_data = save_node_data(ng, root);
    // serialize into archive
    ar(CEREAL_NVP(node_data));
  }

  template <class Archive>
  void load_user_node_graph(
    Archive& ar,
    node_handle& root,
    structured_node_graph& ng)
  {
    // read message from archive
    auto node_data = ngdata();
    ar(CEREAL_NVP(node_data));
    // write result to references
    load_node_data(node_data, ng, root);
  }

  // explicit instantiation
  template void save_user_node_graph<cereal::JSONOutputArchive>(
    cereal::JSONOutputArchive&,
    const node_handle&,
    const structured_node_graph&);

  // explicit instantiation
  template void load_user_node_graph<cereal::JSONInputArchive>(
    cereal::JSONInputArchive&,
    node_handle&,
    structured_node_graph&);

} // namespace yave

CEREAL_CLASS_VERSION(yave::ngdata, 0);