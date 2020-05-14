//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/lib/vec/vec.hpp>
#include <yave/support/id.hpp>

#include <vector>
#include <memory>

namespace yave::editor {
  // fwd
  class data_context;
  class view_context;
  class render_context;
} // namespace yave::editor

namespace yave::wm {

  namespace events {
    // fwd
    class mouse_click;
    class mouse_double_click;
    class mouse_hover;
    class key_press;
    class key_release;
    class key_repeat;
  } // namespace events

  /// Base class of GUI window and widgets
  class window
  {
    friend class window_manager;

  public:
    /// ctor
    window(std::string name, fvec2 pos, fvec2 size);
    /// dtor
    virtual ~window() noexcept;

  public: /* recursive functions */
    /// render content.
    virtual void render(editor::render_context& render_ctx) const = 0;
    /// resize window. this function will be called from update().
    virtual void resize(const fvec2& pos, const fvec2& size) = 0;
    /// update window based on view model
    virtual void update(
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) = 0;

  public: /* event functions */
    // emit new event
    void emit(
      event& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// click
    virtual void on_mouse_click(
      events::mouse_click& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// double click
    virtual void on_mouse_double_click(
      events::mouse_double_click& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// hover
    virtual void on_mouse_hover(
      events::mouse_hover& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// key press
    virtual void on_key_press(
      events::key_press& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// key release
    virtual void on_key_release(
      events::key_release& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// key repeat
    virtual void on_key_repeat(
      events::key_repeat& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

    /// custom event
    virtual void on_custom_event(
      event& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const;

  public:
    auto& parent() const
    {
      return m_parent;
    }

    auto& id() const
    {
      return m_id;
    }

    auto& name() const
    {
      return m_name;
    }

    auto& pos() const
    {
      return m_pos;
    }

    auto& size() const
    {
      return m_size;
    }

    auto& children() const
    {
      return m_children;
    }

  private:
    /// parent pointer
    window* m_parent;
    /// id
    uid m_id;
    /// child windows
    std::vector<std::unique_ptr<window>> m_children;

  protected:
    /// name
    std::string m_name;
    /// window size
    fvec2 m_pos, m_size;
  };

  /// Create new window
  template <class W, class... Args>
  [[nodiscard]] auto make_window(Args&&... args)
  {
    return std::make_unique<W>(std::forward<Args>(args)...);
  }

} // namespace yave::wm