//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/overloaded.hpp>
#include <yave/node/compiler/location.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>

#include <variant>
#include <yave/support/overloaded.hpp>

namespace yave {

  /// ty2 dynamic typing module
  namespace ty2 {

    struct tcon;
    struct tap;
    struct tvar;
    struct ntype;

    /// x | t t | con(t1, t2, ...)
    using node_type = std::variant<tvar, tap, tcon>;

    /// node type
    using NodeType = Box<node_type>;

  } // namespace ty2
} // namespace yave

YAVE_DECL_TYPE(yave::ty2::NodeType, "c3264f3b-b427-4501-9fff-06851e14beec");

namespace yave::ty2 {

  /// type location
  struct location
  {
    // source socket
    socket_handle socket;
  };

  /// ap
  struct tap
  {
    // location
    location loc;
    // app
    object_ptr<const NodeType> app;
    // arg
    object_ptr<const NodeType> arg;
  };

  /// var
  struct tvar
  {
    // location
    location loc;
    // var type
    object_ptr<const Type> var;
  };

  /// con
  struct tcon
  {
    // location
    location loc;
    // type constructor
    object_ptr<const Type> con;
  };

  /// convert normal type tree to node type tree
  inline auto from_obj_type(const object_ptr<const Type>& tp, location loc = {})
    -> object_ptr<NodeType>
  {
    if (is_tcon_type(tp)) {
      return make_object<NodeType>(node_type {tcon {loc, tp}});
    }

    if (is_tvar_type(tp)) {
      return make_object<NodeType>(node_type {tvar {loc, tp}});
    }

    if (auto ap = get_if<tap_type>(tp.value())) {
      return make_object<NodeType>(
        node_type {tap {loc, from_obj_type(ap->t1), from_obj_type(ap->t2)}});
    }
    unreachable();
  }

  /// make tcon for arrow type
  inline auto arrow_type_tcon(location loc = {})
  {
    return make_object<NodeType>(tcon {loc, yave::arrow_type_tcon()});
  }

  /// make arrow type
  inline auto make_arrow_type(
    const object_ptr<const NodeType>& t1,
    const object_ptr<const NodeType>& t2,
    location loc = {})
  {
    return make_object<NodeType>(tap {
      loc, make_object<NodeType>(tap {loc, arrow_type_tcon(loc), t1}), t2});
  }

  /// check type equailty
  inline bool same_type(
    const object_ptr<const NodeType>& t1,
    const object_ptr<const NodeType>& t2)
  {
    return std::visit(
      overloaded {
        [](tcon& l, tcon& r) { return same_type(l.con, r.con); },
        [](tvar& l, tvar& r) { return same_type(l.var, r.var); },
        [](tap& l, tap& r) {
          return same_type(l.app, r.app) && same_type(l.arg, r.arg);
        },
        [](auto&, auto&) { return false; }},
      *t1,
      *t2);
  }

  /// get location of type
  inline auto location_of(const object_ptr<const NodeType>& type)
  {
    return type ? std::visit([](auto&& t) { return t.loc; }, *type)
                : location();
  }

  /// substitution
  struct subst
  {
    /// src (tvar)
    object_ptr<const NodeType> src;
    /// dst
    object_ptr<const NodeType> dst;
  };

  inline auto apply_subst_impl(
    const subst& s,
    const object_ptr<const NodeType>& ty) -> object_ptr<const NodeType>
  {
    if (auto ap = std::get_if<tap>(ty.value())) {

      auto t1 = apply_subst_impl(s, ap->app);
      auto t2 = apply_subst_impl(s, ap->arg);

      return (!t1 && !t2) ? nullptr
                          : make_object<NodeType>(tap {
                            ap->loc, t1 ? t1 : ap->app, t2 ? t2 : ap->arg});
    }

    if (same_type(ty, s.src))
      return s.dst;

    return nullptr;
  }

  inline auto apply_subst(const subst& s, const object_ptr<const NodeType>& ty)
  {
    assert(std::get_if<tvar>(s.src.value()));

    if (auto r = apply_subst_impl(s, ty))
      return r;

    return ty;
  }

  /// assumption
  struct assmp
  {
    /// identifier (tvar)
    uid id;
    /// type
    object_ptr<const NodeType> type;
  };

  class assmp_map
  {
    // mapping from identifier to type
    std::map<uid, object_ptr<const NodeType>> m_map;

  public:
    assmp_map()            = default;
    assmp_map(assmp_map&&) = default;
    assmp_map& operator=(assmp_map&&) = default;

    void insert(const assmp& a)
    {
      auto p = m_map.try_emplace(a.id, a.type);

      if (!p.second)
        throw std::logic_error("duplicated type assumption");
    }

    void erase(uid id)
    {
      m_map.erase(id);
    }

    auto find(uid id) const -> std::optional<assmp>
    {
      auto it = m_map.find(id);

      if (it == m_map.end())
        return std::nullopt;

      return assmp {it->first, it->second};
    }

    template <class F>
    void for_each(F&& func) const
    {
      for (auto&& p : m_map) {
        std::forward<F>(func)(p.first, p.second);
      }
    }

    template <class F>
    void for_each(F&& func)
    {
      for (auto&& p : m_map) {
        std::forward<F>(func)(p.first, p.second);
      }
    }
  };

  struct typing_environment
  {
    /// type assumptions
    assmp_map envA;
    /// overloading assumptions
    assmp_map envB;
    /// locator
    location_map locator;

    /// get node type with location
    auto get_type(const object_ptr<const Object>& obj) const
    {
      auto ty =
        from_obj_type(yave::get_type(obj), location {locator.locate(obj)});
      return ty;
    }

    /// create fresh type variable
    auto genvar(const object_ptr<const Object>& obj) const
    {
      return make_object<NodeType>(
        tvar {.loc = location {locator.locate(obj)}, .var = yave::genvar()});
    }

    /// create fresh identifier
    auto genid() const
    {
      return uid::random_generate();
    }

    auto add_location(
      const object_ptr<const NodeType>& type,
      const object_ptr<const Object>& obj) const
    {
      auto ret = type.clone();

      // FIXME: set location recursively?
      std::visit(
        [&](auto& t) { t.loc = location {locator.locate(obj)}; }, *ret);

      return ret;
    }

    /// create fresh polymorphic type
    auto genpoly(const object_ptr<const NodeType>& ty)
    {
    }
  };

  struct typing_result
  {
    /// monomorphized apply tree
    object_ptr<const Object> obj;
    /// type of apply tree
    object_ptr<const Type> type;
  };

  inline auto type_of_impl(
    const object_ptr<const Object>& obj,
    typing_environment& env) -> object_ptr<const NodeType>
  {
    if (auto apply = value_cast_if<Apply>(obj)) {

      auto& storage = _get_storage(*apply);

      if (storage.is_result())
        return type_of_impl(storage.get_result(), env);

      auto t1 = type_of_impl(storage.app(), env);

      auto t1id = env.genid();
      env.envA.insert({t1id, t1});

      auto t2 = type_of_impl(storage.arg(), env);

      t1 = env.envA.find(t1id)->type;
      env.envA.erase(t1id);

      auto var = env.genvar(obj);
      auto as  = unify(t1, make_arrow_type(t2, var));
      auto ty  = apply_subst(as, var);

      as.erase(var);
    }

    // Lambda
    if (auto lambda = value_cast_if<Lambda>(obj)) {

      auto& storage = _get_storage(*lambda);

      auto loc = location {env.locator.locate(obj)};

      auto id  = uid {storage.var->id()};
      auto var = from_obj_type(make_var_type(id.data), loc);

      env.envA.insert({id, var});

      auto t1 = type_of_impl(storage.var, env);
      auto t2 = type_of_impl(storage.body, env);

      auto aS = subst {var, env.envA.find(id)->type};
      auto ty = make_arrow_type(apply_subst(aS, t1), t2, loc);

      env.envA.erase(id);

      return ty;
    }

    if (auto variable = value_cast_if<Variable>(obj)) {

      if (auto a = env.envA.find({variable->id()}))
        return a->type;

      // fail
      auto var = make_var_type(variable->id());
      throw type_error::unbounded_variable(std::move(var), obj);
    }
  }

  /// \param obj apply tree
  /// \param loc_map location map from socket to instance
  [[nodiscard]] inline auto type_of(
    const object_ptr<const Object>& obj,
    const class_env& overloadings,
    const location_map& loc_map) -> typing_result
  {
  }
} // namespace yave::ty2