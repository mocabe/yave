//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/executable.hpp>
#include <yave/obj/frame/frame.hpp>

#include <yave/rts/rts.hpp>

namespace yave {

  executable::executable(
    object_ptr<const Object> obj,
    object_ptr<const Type> type)
    : m_obj {std::move(obj)}
    , m_type {std::move(type)}
  {
  }

  executable::executable(const executable& other)
    : m_obj {other.m_obj}
    , m_type {other.m_type}
  {
  }

  executable::executable(executable&& other)
    : m_obj {std::move(other.m_obj)}
    , m_type {std::move(other.m_type)}
  {
  }

  executable& executable::operator=(const executable& other)
  {
    m_obj  = other.m_obj;
    m_type = other.m_type;
    return *this;
  }

  executable& executable::operator=(executable&& other)
  {
    m_obj  = std::move(other.m_obj);
    m_type = std::move(other.m_type);
    return *this;
  }

  object_ptr<const Type> executable::type() const
  {
    return m_type;
  }

  object_ptr<const Object> executable::object() const
  {
    return m_obj;
  }

  object_ptr<const Object> executable::execute(yave::frame frame)
  {
    auto app = m_obj << make_object<Frame>(frame);
    return eval(app);
  }
}