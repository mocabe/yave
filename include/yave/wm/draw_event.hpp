//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/window_traverser.hpp>

namespace yave::wm {

  namespace events {

    /// draw event
    class draw final : public event
    {
      fvec2 m_clip_pos, m_clip_ext;

      using event::accept;
      using event::accepted;

    public:
      draw(fvec2 clip_pos, fvec2 clip_ext)
        : event::event()
        , m_clip_pos {clip_pos}
        , m_clip_ext {clip_ext}
      {
      }

      /// window clip rect pos
      auto& clip_pos() const
      {
        return m_clip_pos;
      }

      /// window clip rect extent
      auto& clip_ext() const
      {
        return m_clip_ext;
      }
    };

  } // namespace events

  class draw_event_dispatcher : event_dispatcher
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    draw_event_dispatcher(
      const editor::data_context& dctx,
      const editor::view_context& vctx);

    ~draw_event_dispatcher() noexcept;

    void dispatch(window* root) override;
  };
}