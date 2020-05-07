//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/rts/kinds.hpp>
#include <yave/rts/types.hpp>
#include <yave/rts/type_gen.hpp>

using namespace yave;

void test_is_kind()
{
  static_assert(is_kind(type_c<kstar>));
  static_assert(is_kind(type_c<kfun<kstar, kstar>>));
  static_assert(is_kind(make_kind()));
  static_assert(is_kind(make_kind(make_kind(), make_kind())));
}

void test_kind_of()
{
  static_assert(kind_of(type_c<ty_value<int>>) == type_c<kstar>);
  static_assert(
    kind_of(type_c<ty_arrow<ty_value<int>, ty_value<double>>>)
    == type_c<kstar>);
  static_assert(kind_of(type_c<ty_list<ty_value<int>>>) == type_c<kstar>);
}

void test_is_ty()
{
  static_assert(is_tvar(type_c<tvar<int, kstar>>));
  static_assert(is_tcon(type_c<ty_value<int>>));
  static_assert(is_tcon(type_c<tcon<void, void>>));
}

void test_ty()
{
  using tArrow = tcon<arrow_tcon_tag, kfun<kstar, kfun<kstar, kstar>>>;
  using tList  = tcon<list_tcon_tag, kfun<kstar, kstar>>;
  using tInt   = tcon<value_tcon_tag<int>, kstar>;

  class a;

  static_assert(
    type_c<tap<
      tap<tArrow, tInt>,
      tap<
        tList,
        tvar<
          a,
          kstar>>>> == type_c<ty_arrow<ty_value<int>, ty_list<ty_var<a>>>>);
}

void test_kind_gen()
{
  static_assert(!detail::kind_address<kstar>());
  static_assert(detail::kind_address<kfun<kfun<kstar, kstar>, kstar>>());
  static_assert(
    detail::kind_address<kfun<kfun<kstar, kstar>, kfun<kstar, kstar>>>());
}