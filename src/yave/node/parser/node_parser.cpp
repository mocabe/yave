//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/core/node_group.hpp>
#include <yave/support/log.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>
#include <tl/optional.hpp>

YAVE_DECL_G_LOGGER(node_parser)

// cursed, but very useful
#define mem_fn(FN) \
  [this](auto&&... args) { return FN(std::forward<decltype(args)>(args)...); }

namespace yave {

  namespace rn = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;

  // tl::optional -> std::optional
  template <class T>
  constexpr auto to_std(tl::optional<T>&& opt) -> std::optional<T>
  {
    if (opt)
      return std::move(*opt);
    else
      return std::nullopt;
  }

  class node_parser::impl
  {
  public:
    mutable std::mutex mtx = {};
    error_list errors      = {};

  public:
    impl()           = default;
    ~impl() noexcept = default;

  private:
    auto lock() const
    {
      return std::unique_lock {mtx};
    }

    auto validate(structured_node_graph&& ng)
      -> tl::optional<structured_node_graph>
    {
      auto roots = ng.search_path("/");

      auto root = [&] {
        for (auto&& r : roots)
          if (ng.get_name(r) == "root")
            return r;
        return node_handle();
      }();

      if (!root) {
        errors.push_back(
          make_error<parse_error::unexpected_error>("Root node not found"));
        return tl::nullopt;
      }

      if (ng.output_sockets(root).size() != 1) {
        errors.push_back(make_error<parse_error::unexpected_error>(
          "Root should have single output"));
        return tl::nullopt;
      }

      if (ng.input_sockets(root).size() != 0) {
        errors.push_back(make_error<parse_error::unexpected_error>(
          "Root should not have input"));
        return tl::nullopt;
      }

      // node -> [socket]
      using memo = std::map<uid, std::vector<uid>>;

      struct
      {
        bool marked(const node_handle& n, const socket_handle& s, const memo& m)
        {
          if (auto it = m.find(n.id()); it != m.end())
            return rn::find(it->second, s.id()) != it->second.end();
          return false;
        }

        void mark(const node_handle& n, const socket_handle& s, memo& m)
        {
          assert(!marked(n, s, m));
          if (auto it = m.find(n.id()); it != m.end())
            it->second.push_back(s.id());
          else
            m.insert({n.id(), {s.id()}});
        }

        // general node check
        void chk_n(
          const node_handle& n,
          const socket_handle& os,
          const structured_node_graph& ng,
          error_list& es,
          memo& m)
        {
          // IO will be checked by rec_g
          assert(ng.is_group_member(n));

          if (marked(n, os, m))
            return;

          auto ics = ng.input_connections(n);
          auto iss = ng.input_sockets(n);

          auto missing = [&](auto&& s) {
            return !ng.get_data(s) && ng.connections(s).empty();
          };

          size_t cnt = rn::count_if(iss, missing);

          if (cnt != iss.size())
            for (auto&& s : iss)
              if (missing(s))
                es.push_back(
                  make_error<parse_error::missing_input>(n.id(), s.id()));

          mark(n, os, m);
        }

        void rec_n(
          const node_handle& n,
          const socket_handle& os,
          const structured_node_graph& ng,
          error_list& es,
          memo& m)
        {
          if (ng.is_group(n))
            return rec_g(n, os, ng, es, m);

          if (ng.is_function(n))
            return rec_f(n, os, ng, es, m);

          if (ng.is_group_input(n))
            return;

          unreachable();
        }

        // check group and its inside
        void rec_g(
          const node_handle& g,
          const socket_handle& os,
          const structured_node_graph& ng,
          error_list& es,
          memo& m)
        {
          assert(ng.is_group(g));

          // check interface
          chk_n(g, os, ng, es, m);

          // check input
          for (auto&& c : ng.input_connections(g)) {
            auto ci = ng.get_info(c);
            rec_n(ci->src_node(), ci->src_socket(), ng, es, m);
          }

          // check inside
          {
            auto go  = ng.get_group_output(g);
            auto idx = *ng.get_index(os);
            auto s   = ng.input_sockets(go)[idx];

            if (ng.connections(s).empty())
              es.push_back(
                make_error<parse_error::missing_input>(go.id(), s.id()));

            for (auto&& c : ng.connections(s)) {
              auto ci = ng.get_info(c);
              rec_n(ci->src_node(), ci->src_socket(), ng, es, m);
            }
          }
        }

        // check function
        void rec_f(
          const node_handle& f,
          const socket_handle& s,
          const structured_node_graph& ng,
          error_list& es,
          memo& m)
        {
          chk_n(f, s, ng, es, m);

          // check input
          for (auto&& c : ng.input_connections(f)) {
            auto ci = ng.get_info(c);
            rec_n(ci->src_node(), ci->src_socket(), ng, es, m);
          }
        }

      } impl;

      memo m;
      impl.rec_n(root, ng.output_sockets(root)[0], ng, errors, m);

      if (errors.empty())
        return std::move(ng);

      return tl::nullopt;
    }

  public:
    auto parse(structured_node_graph&& ng)
      -> tl::optional<structured_node_graph>
    {
      auto lck = lock();
      errors.clear();

      return tl::make_optional(std::move(ng))
        .and_then(mem_fn(validate))
        .or_else([&] {
          Error(g_logger, "Failed to parse node graph");
          for (auto&& e : errors) {
            Error(g_logger, "error: {}", e.message());
          }
        });
    }

    auto get_errors()
    {
      auto lck = lock();
      return errors.clone();
    }
  };

  node_parser::node_parser()
    : m_pimpl {std::make_unique<impl>()}
  {
    init_logger();
  }

  node_parser::~node_parser() noexcept = default;

  auto node_parser::get_errors() const -> error_list
  {
    return m_pimpl->get_errors();
  }

  auto node_parser::parse(structured_node_graph&& ng)
    -> std::optional<structured_node_graph>
  {
    return to_std(m_pimpl->parse(std::move(ng)));
  }

} // namespace yave