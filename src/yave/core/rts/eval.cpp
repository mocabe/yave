
#include <yave/core/rts/eval.hpp>

namespace yave {

  /// copy apply graph
  [[nodiscard]] object_ptr<const Object>
    copy_apply_graph(const object_ptr<const Object>& obj)
  {
    if (auto apply = value_cast_if<Apply>(obj)) {
      auto& apply_storage = _get_storage(*apply);
      // return cached value
      if (apply_storage.evaluated()) {
        return apply_storage.get_cache();
      }
      // create new apply
      return make_object<Apply>(
        copy_apply_graph(apply_storage.app()),
        copy_apply_graph(apply_storage.arg()));
    }
    return obj;
  }

  namespace detail {

    /// eval implementation
    [[nodiscard]] object_ptr<const Object>
      eval_obj(const object_ptr<const Object>& obj)
    {
      // detect exception
      if (unlikely(has_exception_tag(obj)))
        throw result_error::exception_result(
          clear_pointer_tag(get_tagged_exception(obj)));

      // apply
      if (auto apply = value_cast_if<Apply>(obj)) {

        // alias: internal storage
        auto& apply_storage = _get_storage(*apply);

        // graph reduction
        if (apply_storage.evaluated()) {
          return apply_storage.get_cache();
        }

        // whnf
        auto app = eval_obj(apply_storage.app());

        // alias: argument
        const auto& arg = apply_storage.arg();
        // alias: app closure
        auto capp = static_cast<const Closure<>*>(app.get());

        /*
          These exceptions should not triggered on well-typed input. Just
          leaving it here to avoid catastrophic heap corruption when something
          went totally wrong.
        */
        if (unlikely(!has_arrow_type(app))) {
          throw eval_error::bad_apply();
        }
        if (unlikely(capp->arity() == 0)) {
          throw eval_error::too_many_arguments();
        }

        // clone closure and apply
        auto ret = [&] {
          // clone
          auto pap = clone(app);
          // alias: pap closure
          auto cpap = static_cast<const Closure<>*>(pap.get());

          // push argument
          auto arity       = --cpap->arity();
          cpap->arg(arity) = arg;

          // call code()
          if (unlikely(arity == 0)) {
            return eval_obj(cpap->code());
          }

          return pap;
        }();

        // set cache
        apply_storage.set_cache(ret);

        return ret;
      }

      return obj;
    }

  } // namespace detail

} // namespace yave