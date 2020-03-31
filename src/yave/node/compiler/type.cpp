//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/type.hpp>
#include <yave/rts/value_cast.hpp>

namespace yave {

  auto class_env::add_overloading(
    const std::vector<object_ptr<const Object>>& instances)
    -> object_ptr<const Overloaded>
  {
    auto src = make_object<Overloaded>();
    auto id  = src->id_var;

    if (m_map.find(id) != m_map.end())
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
          (void)unify({{t, instp}}, nullptr);
          throw std::invalid_argument("Overlapping class instance");
        } catch (type_error::type_error&) {
          // ok
        }
      }
      types.push_back(instp);
    }

    // add
    m_map.emplace(id, overloaded_class {gentp, instances});

    return src;
  }

  auto class_env::find_overloading(const object_ptr<const Type>& class_id) const
    -> const overloaded_class*
  {
    assert(is_var_type(class_id));

    auto it = m_map.find(class_id);

    if (it == m_map.end())
      return nullptr;

    return &it->second;
  }

  namespace {

    /// typing environment for overloaded extension
    struct overloading_env
    {
      overloading_env(class_env&& env)
        : classes {std::move(env)}
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

      /// result overloaded candidate.
      /// map of (overloaded, instance)
      std::map<object_ptr<const Object>, object_ptr<const Object>> results;

      /// Instantiate class
      auto instantiate_class(const object_ptr<const Overloaded>& src)
        -> object_ptr<const Type>
      {
        auto overload = classes.find_overloading(src->id_var);

        if (!overload)
          throw type_error::type_error(src, "Invalid class ID");

        auto var = genvar();
        auto tp  = genpoly(overload->type, envA);

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
      object_ptr<const Type> ty,
      const object_ptr<const Object>& src) -> object_ptr<const Type>
    {
      // lsit of closed assumptions
      std::vector<object_ptr<const Type>> closed;

      env.envB.for_each([&](auto& from, auto& to) {
        // ignore assumptions which contains variable.
        // this is probably not ideal way, but should work fairly well.
        if (!vars(to).empty())
          return;

        // find overloading candidates
        assert(env.references.find(from));
        assert(env.sources.find(from) != env.sources.end());
        auto class_id  = env.references.find(from)->to;
        auto class_val = *env.classes.find_overloading(class_id);
        auto source    = env.sources.find(from)->second;

        // find specializable overloadings

        object_ptr<const Type> result_type   = nullptr;
        object_ptr<const Object> result_inst = nullptr;

        for (auto&& inst : class_val.instances) {
          auto insty = genpoly(get_type(inst), env.envA);

          if (specializable(insty, to)) {
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
          throw type_error::no_valid_overloading(src);

        // get substitition to fix
        // use empty set if it's not specializable
        type_arrow_map subst;
        if (auto tmp = specializable(to, result_type))
          subst = std::move(*tmp);

        // update A and B
        subst.for_each([&](auto& from, auto& to) {
          apply_subst(env.envA, {from, to});
          apply_subst(env.envB, {from, to});
        });
        // update ty
        ty = subst_type_all(subst, ty);

        // cache result
        env.results.emplace(source, result_inst);
        closed.push_back(from);
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

        auto var = genvar();
        auto cs =
          std::vector {type_constr {subst_type_all(env.envA, t1),
                                    make_object<Type>(arrow_type {t2, var})}};
        auto as = unify(std::move(cs), obj);
        auto ty = subst_type_all(as, var);

        as.erase(var);
        as.for_each([&](auto& from, auto& to) {
          compose_subst(env.envA, {from, to});
          apply_subst(env.envB, {from, to});
        });

        // only close when envA has no free variable
        if (vars(env.envA).empty())
          ty = close_assumption(env, ty, obj);

        return ty;
      }

      // Lambda
      if (auto lambda = value_cast_if<Lambda>(obj)) {

        auto& storage = _get_storage(*lambda);

        auto var = make_object<Type>(var_type {storage.var->id()});
        env.envA.insert(type_arrow {var, var});

        auto t1 = type_of_overloaded_impl(storage.var, env);
        auto t2 = type_of_overloaded_impl(storage.body, env);

        auto ty =
          make_object<Type>(arrow_type {subst_type_all(env.envA, t1), t2});

        env.envA.erase(t1);

        return ty;
      }

      // Variable
      if (auto variable = value_cast_if<Variable>(obj)) {

        auto var = make_object<Type>(var_type {variable->id()});
        if (auto s = env.envA.find(var))
          return s->to;

        throw type_error::unbounded_variable(obj, var);
      }

      // Overloaded
      if (auto overloaded = value_cast_if<Overloaded>(obj)) {
        return env.instantiate_class(overloaded);
      }

      // arrow -> arrow or PAP
      if (has_arrow_type(obj)) {
        auto c = reinterpret_cast<const Closure<>*>(obj.get());
        // pap: return root apply node
        // app: get_type
        return c->is_pap()
                 ? type_of_overloaded_impl(c->vertebrae(c->arity), env)
                 : genpoly(get_type(obj), env.envA);
      }

      // list:
      //  | []   : [] a
      //  | x:xs : [type_of(x)]
      if (has_list_type(obj)) {

        auto list = static_object_cast<const List<Object>>(obj);

        auto& storage = _get_storage(*list);

        if (storage.is_nil())
          return get_type(obj);
        else
          return make_object<Type>(
            list_type {type_of_overloaded_impl(storage.car, env)});
      }

      // value -> value
      if (has_value_type(obj))
        return get_type(obj);

      // var -> var
      if (has_var_type(obj))
        return get_type(obj);

      unreachable();
    }

    /// Duplicates call for overloaded function.
    /// To resolve overloading, we need unique identifier for each occurence
    /// of overloaded call, one of the easiest (but rather stupid) way is
    /// duplicate entire subtree with overloaded call so each Overloaded node
    /// can have unique address.
    inline auto duplicate_overloads(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
    {
      if (auto apply = value_cast_if<Apply>(obj)) {
        auto& storage = _get_storage(*apply);
        return make_object<Apply>(
          duplicate_overloads(storage.app()),
          duplicate_overloads(storage.arg()));
      }

      if (auto lambda = value_cast_if<Lambda>(obj)) {
        auto& storage = _get_storage(*lambda);
        return make_object<Lambda>(
          storage.var, duplicate_overloads(storage.body));
      }

      if (auto overloaded = value_cast_if<Overloaded>(obj)) {
        return overloaded.clone();
      }

      return obj;
    }

    inline auto rebuild_overloads(
      const object_ptr<const Object>& obj,
      const overloading_env& env) -> object_ptr<const Object>
    {
      if (!env.envB.empty())
        throw type_error::no_valid_overloading(obj);

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

        return overloaded;
      }

      return obj;
    }
  } // namespace

  auto type_of_overloaded(
    const object_ptr<const Object>& obj,
    class_env classes)
    -> std::pair<object_ptr<const Type>, object_ptr<const Object>>
  {
    overloading_env env(std::move(classes));
    auto tree = duplicate_overloads(obj);
    auto ty   = type_of_overloaded_impl(tree, env);
    ty        = close_assumption(env, ty, tree);
    return {ty, rebuild_overloads(tree, env)};
  }

} // namespace yave