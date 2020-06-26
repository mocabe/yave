//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/type.hpp>
#include <yave/node/compiler/errors.hpp>
#include <yave/rts/value_cast.hpp>

namespace yave {

  auto class_env::add_overloading(
    const uid& id,
    const std::vector<object_ptr<const Object>>& instances)
    -> object_ptr<const Overloaded>
  {
    auto src    = make_object<Overloaded>(id.data);
    auto id_var = src->id_var;

    if (m_map.find(id_var) != m_map.end())
      throw std::invalid_argument("Class is already defined");

    // calculate generalized type
    std::vector<object_ptr<const Type>> types;
    for (auto&& inst : instances) {
      types.push_back(get_type(inst));
    }
    auto gentp = generalize(types);

    // check overlap
    types.clear();
    for (auto&& inst : instances) {

      auto instp = get_type(inst);

      if (types.empty()) {
        types.push_back(instp);
        continue;
      }

      for (auto&& t : types) {
        try {
          (void)unify(t, instp);
          throw std::invalid_argument("Overlapping class instance");
        } catch (type_error::type_error&) {
          // ok
        }
      }
      types.push_back(instp);
    }

    // add
    m_map.emplace(id_var, overloaded_class {gentp, instances});

    return src;
  }

  auto class_env::find_overloaded(const uid& id) const
    -> object_ptr<const Overloaded>
  {
    auto id_var = make_var_type(id.data);

    if (m_map.find(id_var) != m_map.end())
      // always create new object
      return make_object<Overloaded>(id.data);

    return nullptr;
  }

  auto class_env::find_overloading(const object_ptr<const Type>& class_id) const
    -> const overloaded_class*
  {
    assert(is_tvar_type(class_id));

    auto it = m_map.find(class_id);

    if (it == m_map.end())
      return nullptr;

    return &it->second;
  }

  namespace {

    /// typing environment for overloaded extension
    struct overloading_env
    {
      overloading_env(class_env&& env, location_map&& loc)
        : classes {std::move(env)}
        , locations {std::move(loc)}
      {
      }

      /// normal type environment.
      /// map of (tyvar, type)
      type_arrow_map envA;

      /// overloading assumptions.
      /// map of (tyvar, assumption)
      type_arrow_map envB;

      /// overloading references
      /// map of (tyvar, class ID)
      type_arrow_map references;

      /// overloading srouces
      /// map of (tyvar, srouce node)
      std::map<object_ptr<const Type>, object_ptr<const Object>, var_type_comp>
        sources;

      /// overloaded classes.
      /// map of (class ID, class)
      class_env classes;

      /// location map
      location_map locations;

      /// result overloaded candidate.
      /// map of (overloaded, instance)
      std::map<object_ptr<const Object>, object_ptr<const Object>> results;

      /// Create new type variable
      /// \param src located object of which location will be propagated to new
      /// type variable.
      auto genvar(const object_ptr<const Object>& src)
      {
        auto var = yave::genvar();
        locations.add_location(var, locations.locate(src));
        return var;
      }

      /// Create fresh polymorphic type
      auto genpoly(const object_ptr<const Type>& tp)
      {
        auto vs = vars(tp);
        auto t  = tp;

        for (auto v : vs) {

          if (envA.find(v))
            continue;

          auto a = type_arrow {v, this->genvar(v)};
          t      = apply_type_arrow(a, t);
        }
        return t;
      }

      /// Get type of object
      auto get_type(const object_ptr<const Object>& obj)
      {
        auto ty = copy_type(yave::get_type(obj));
        locations.add_location(ty, locations.locate(obj));
        return ty;
      }

      /// Instantiate class
      auto instantiate_class(const object_ptr<const Overloaded>& src)
        -> object_ptr<const Type>
      {
        auto overload = classes.find_overloading(src->id_var);

        if (!overload)
          throw compile_error::unexpected_error(
            locations.locate(src),
            "Could not instantiate overloading: Invalid class ID");

        auto var = this->genvar(src);
        auto tp  = this->genpoly(overload->type);

        envB.insert({var, tp});
        references.insert({var, src->id_var});
        sources.insert({var, src});

        return tp;
      }
    };

    // ------------------------------------------
    // type_of_overloaded

    /// close assumption of overloading
    inline auto close_assumption(
      overloading_env& env,
      object_ptr<const Type> ty) -> object_ptr<const Type>
    {
      // lsit of closed assumptions
      std::vector<object_ptr<const Type>> closed;

      env.envB.for_each([&](auto& tv, auto& assump) {
        // ignore assumptions which contains variable.
        // this is probably not ideal way, but should work fairly well.
        if (!vars(assump).empty())
          return;

        // find overloading candidates
        assert(env.references.find(tv));
        assert(env.sources.find(tv) != env.sources.end());
        auto class_id  = env.references.find(tv)->t2;
        auto class_val = *env.classes.find_overloading(class_id);
        auto source    = env.sources.find(tv)->second;

        // find specializable overloadings

        object_ptr<const Type> result_type   = nullptr;
        object_ptr<const Object> result_inst = nullptr;

        for (auto&& inst : class_val.instances) {
          auto insty = env.genpoly(env.get_type(inst));

          if (specializable(insty, assump)) {
            // ambiguous
            if (result_type)
              return;
            // first find
            result_type = insty;
            result_inst = inst;
          }
        }

        // could not match overloading
        if (!result_type)
          throw compile_error::no_valid_overloading(env.locations.locate(tv));

        // get substitition to fix
        // use empty set if it's not specializable
        type_arrow_map subst;
        if (auto tmp = specializable(assump, result_type))
          subst = std::move(*tmp);

        // update A and B
        env.envA.for_each(
          [&](auto&, auto& t2) { t2 = apply_subst(subst, t2); });
        env.envB.for_each(
          [&](auto&, auto& t2) { t2 = apply_subst(subst, t2); });

        // update ty
        ty = apply_subst(subst, ty);

        // cache result
        env.results.emplace(source, result_inst);
        closed.push_back(tv);
      });

      // remove assumptions no longer required
      for (auto&& i : closed) {
        env.envB.erase(i);
        env.references.erase(i);
        env.sources.erase(i);
      }

      return ty;
    }

    inline auto type_of_overloaded_impl(
      const object_ptr<const Object>& obj,
      overloading_env& env) -> object_ptr<const Type>
    {
      // Apply
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& storage = _get_storage(*apply);

        // cached
        if (storage.is_result())
          return type_of_overloaded_impl(storage.get_result(), env);

        auto t1 = type_of_overloaded_impl(storage.app(), env);
        auto t2 = type_of_overloaded_impl(storage.arg(), env);

        try {

          auto var = env.genvar(obj);
          auto as  = unify(apply_subst(env.envA, t1), make_arrow_type(t2, var));
          auto ty  = apply_subst(as, var);

          as.erase(var);

          // update A and B
          compose_subst_over(env.envA, as);
          env.envB.for_each([&](auto&, auto& to) { to = apply_subst(as, to); });

          // only close when envA has no free variable
          if (vars(env.envA).empty())
            ty = close_assumption(env, ty);

          return ty;

        } catch (type_error::type_missmatch& e) {
          throw compile_error::type_missmatch(
            env.locations.locate(e.expected()),
            env.locations.locate(e.provided()),
            e.expected(),
            e.provided());
        } catch (type_error::unsolvable_constraints& e) {
          throw compile_error::unsolvable_constraints(
            env.locations.locate(e.t1()),
            env.locations.locate(e.t2()),
            e.t1(),
            e.t2());
        } catch (type_error::type_error& e) {
          throw; // TODO catch other type errors
        }
      }

      // Lambda
      if (auto lambda = value_cast_if<Lambda>(obj)) {

        auto& storage = _get_storage(*lambda);

        auto var = make_var_type(storage.var->id());
        env.envA.insert(type_arrow {var, var});

        auto t1 = type_of_overloaded_impl(storage.var, env);
        auto t2 = type_of_overloaded_impl(storage.body, env);

        auto ty = make_arrow_type(apply_subst(env.envA, t1), t2);
        env.locations.add_location(ty, env.locations.locate(obj));

        env.envA.erase(t1);

        return ty;
      }

      // Variable
      if (auto variable = value_cast_if<Variable>(obj)) {

        auto var = make_var_type(variable->id());
        env.locations.add_location(var, env.locations.locate(obj));

        if (auto s = env.envA.find(var))
          return s->t2;

        throw type_error::unbounded_variable(var, obj);
      }

      // Overloaded
      if (auto overloaded = value_cast_if<Overloaded>(obj))
        return env.instantiate_class(overloaded);

      // Partially applied closures
      if (auto c = value_cast_if<Closure<>>(obj))
        if (c->is_pap())
          return type_of_overloaded_impl(c->vertebrae(c->arity), env);

      // tap
      if (has_tap_type(obj))
        return env.genpoly(env.get_type(obj));

      // tcon
      if (has_tcon_type(obj))
        return env.get_type(obj);

      // tvar
      if (has_tvar_type(obj))
        return env.get_type(obj);

      unreachable();
    }

    auto rebuild_overloads(
      const object_ptr<const Object>& obj,
      const overloading_env& env) -> object_ptr<const Object>
    {
      if (auto apply = value_cast_if<Apply>(obj)) {
        auto& storage = _get_storage(*apply);

        if (storage.is_result())
          return rebuild_overloads(storage.get_result(), env);

        return make_object<Apply>(
          rebuild_overloads(storage.app(), env),
          rebuild_overloads(storage.arg(), env));
      }

      if (auto lambda = value_cast_if<Lambda>(obj)) {
        auto& storage = _get_storage(*lambda);
        return make_object<Lambda>(
          storage.var, rebuild_overloads(storage.body, env));
      }

      if (auto overloaded = value_cast_if<Overloaded>(obj)) {

        auto it = env.results.find(overloaded);

        if (it != env.results.end())
          return it->second;

        throw compile_error::no_valid_overloading(
          env.locations.locate(overloaded));
      }

      return obj;
    }
  } // namespace

  auto type_of_overloaded(
    const object_ptr<const Object>& obj,
    class_env&& classes,
    location_map&& loc)
    -> std::pair<object_ptr<const Type>, object_ptr<const Object>>
  {
    overloading_env env(std::move(classes), std::move(loc));
    auto ty = type_of_overloaded_impl(obj, env);
    ty      = close_assumption(env, ty);
    return {ty, rebuild_overloads(obj, env)};
  }

} // namespace yave