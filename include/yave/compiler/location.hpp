//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/core/fix_lambda.hpp>

#include <map>

namespace yave::compiler {

  /// object/type location map
  class location_map
  {
  public:
    location_map() = default;

    /// add location object
    void add_location(
      const object_ptr<const Object>& obj,
      const socket_handle& h)
    {
      m_map.insert_or_assign(obj.get(), h);
    }

    /// add location to type
    void add_location(
      const object_ptr<const Type>& type,
      const socket_handle& h)
    {
      auto rec = [&](auto self, auto& t, auto& s) -> void {
        m_map.insert_or_assign(t.get(), s);

        if (auto ap = is_tap_type_if(t)) {
          self(ap->t1, s);
          self(ap->t2, s);
        }
      };

      return fix_lambda(rec)(type, h);
    }

    /// get location
    template <class T>
    [[nodiscard]] auto locate(const object_ptr<T>& obj) const
    {
      auto it = m_map.find(obj.get());

      if (it == m_map.end())
        return socket_handle();

      return it->second;
    }

    /// size of registered locations
    [[nodiscard]] auto size() const
    {
      return m_map.size();
    }

  private:
    std::map<const Object*, socket_handle> m_map;
  };

} // namespace yave