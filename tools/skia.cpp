//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <type_traits>
#include <cstdint>

namespace std {

  template <class F, class... ArgTypes>
  using result_of = invoke_result<F, ArgTypes...>;

  template <class T>
  using result_of_t = typename result_of<T>::type;

} // namespace std

#include <skia/core/SkCanvas.h>
#include <skia/core/SkFont.h>
#include <skia/core/SkGraphics.h>
#include <skia/core/SkSurface.h>

int main()
{
}