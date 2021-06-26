//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/uid.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <typeinfo>
namespace yave::editor {
  // fwd
  class data_context;
  class view_context;
} // namespace yave::editor

namespace yave::wm {

  /// Base class of GUI window and widgets
  class window
  {
    /// unique id for this window
    uid m_id;
    /// pointer to parent
    window* m_parent;
    /// child windows
    std::vector<std::unique_ptr<const window>> m_children;
    /// name
    std::string m_name;

  public:
    /// ctor
    window(std::string name);
    /// dtor
    virtual ~window() noexcept;

  protected:
    /// utility function to add new window
    /// \param it iterator position for insertion
    /// \param win window to insert
    void add_any_window(
      typename decltype(m_children)::iterator it,
      std::unique_ptr<window>&& win);

    /// utility function to remove child window
    void remove_any_window(uid id);

  public:
    /// update
    virtual void update(
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) = 0;
    /// draw
    virtual void draw(
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const = 0;

  public:
    /// for linking new child window
    void set_parent(window* new_parent)
    {
      m_parent = new_parent;
    }

    /// get mutable child window pointer
    auto as_mut_child(const std::unique_ptr<const window>& win)
    {
      return const_cast<window*>(win.get());
    }

  public: /* public accessors */
    auto parent() const -> const window*
    {
      return m_parent;
    }

    auto parent()
    {
      return m_parent;
    }

    auto& id() const
    {
      return m_id;
    }

    auto& id()
    {
      return m_id;
    }

    auto& name() const
    {
      return m_name;
    }

    auto& name()
    {
      return m_name;
    }

    auto& children() const
    {
      return m_children;
    }

    auto& children()
    {
      return m_children;
    }

  public:
    /// dynamic casting
    template <class Derived>
    auto as()
    {
      static_assert(std::is_base_of_v<window, Derived>);
      assert(typeid(*this) == typeid(Derived));
      return static_cast<Derived*>(this);
    }

    /// dynamic casting
    template <class Derived>
    auto as() const
    {
      static_assert(std::is_base_of_v<window, Derived>);
      assert(typeid(*this) == typeid(Derived));
      return static_cast<const Derived*>(this);
    }
  };

} // namespace yave::wm