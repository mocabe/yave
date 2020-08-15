//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/window.hpp>
#include <yave/wm/window_visitor.hpp>

namespace yave::wm {

  /// Event visitor
  struct event_visitor : window_visitor
  {
    /// Ctor
    event_visitor(
      std::unique_ptr<event>&& e,
      const editor::data_context& dctx,
      const editor::view_context& vctx);

    /// Dtor
    ~event_visitor() noexcept = default;

    /// Dispatch event to window
    bool visit(window* w) override;

  protected:
    std::unique_ptr<event> m_event;
    const editor::data_context& m_data_ctx;
    const editor::view_context& m_view_ctx;
  };

  /// Event dispatcher
  struct event_dispatcher
  {
    /// dispatch event to window(s)
    virtual void dispatch(window* w) = 0;
  };

  /// = event_visitor + window_traverser
  template <class Visitor, class Traverser>
  class composed_event_dispatcher : public event_dispatcher
  {
    std::pair<Visitor, Traverser> m_pair;

  public:
    template <class... VisitorArgs, class... TraverserArgs>
    composed_event_dispatcher(
      std::tuple<VisitorArgs...> visitor_args,
      std::tuple<TraverserArgs...> traverser_args)
      : m_pair(
        std::piecewise_construct,
        std::move(visitor_args),
        std::move(traverser_args))
    {
    }

    auto& visitor()
    {
      return m_pair.first;
    }

    auto& traverser()
    {
      return m_pair.second;
    }

    virtual void dispatch(window* w) override
    {
      auto& visitor   = m_pair.first;
      auto& traverser = m_pair.second;
      traverser.traverse(w, visitor);
    }
  };

} // namespace yave::wm