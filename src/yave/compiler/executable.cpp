//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/executable.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/rts/rts.hpp>

namespace yave::compiler {

  executable::executable(
    object_ptr<const Object> obj,
    object_ptr<const Type> type)
    : m_obj {std::move(obj)}
    , m_type {std::move(type)}
  {
  }

  executable::executable(executable&& other) noexcept
    : m_obj {std::move(other.m_obj)}
    , m_type {std::move(other.m_type)}
  {
  }

  executable& executable::operator=(executable&& other) noexcept
  {
    m_obj  = std::move(other.m_obj);
    m_type = std::move(other.m_type);
    return *this;
  }

  auto executable::object() const -> const object_ptr<const Object>&
  {
    return m_obj;
  }

  auto executable::type() const -> const object_ptr<const Type>&
  {
    return m_type;
  }

  auto executable::execute(const time& time) -> object_ptr<const Object>
  {
    return eval(
      m_obj << make_object<FrameDemand>(make_object<FrameTime>(time)));
  }

  auto executable::clone() const -> executable
  {
    return {copy_apply_graph(m_obj), m_type};
  }

}