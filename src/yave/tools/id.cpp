//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/tools/id.hpp>

#include <cstring>
#include <random>

#if defined(__MINGW32__)
#  include <windows.h>
#  include <wincrypt.h>
#  include <system_error>
#endif

namespace yave {

  bool operator<(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data < rhs.data;
  }

  bool operator>(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data > rhs.data;
  }

  bool operator<=(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data <= rhs.data;
  }

  bool operator>=(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data >= rhs.data;
  }

  bool operator==(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data == rhs.data;
  }

  std::string to_string(const uid& id)
  {
    return std::to_string(id.data);
  }

  uid uid::random_generate()
  {
#if defined(__MINGW32__)
    HCRYPTPROV hprov;
    if (!CryptAcquireContext(&hprov, nullptr, nullptr, PROV_RSA_FULL, 0)) {
      if (GetLastError() == static_cast<DWORD>(NTE_BAD_KEYSET)) {
        if (!CryptAcquireContext(
              &hprov, nullptr, nullptr, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
          throw std::system_error(
            std::error_code(GetLastError(), std::system_category()),
            "CryptAcquireContext failed.");
        }
      } else {
        throw std::system_error(
          std::error_code(GetLastError(), std::system_category()),
          "CryptAcquireContext failed.");
      }
    }
    uint64_t ret;
    if (!CryptGenRandom(hprov, sizeof(ret), reinterpret_cast<BYTE*>(&ret))) {
      throw std::system_error(
        std::error_code(GetLastError(), std::system_category()),
        "CryptGenRandom failed.");
    }
    CryptReleaseContext(hprov, 0);
    return uid {ret};
#else
    static_assert(std::is_same_v<uint64_t, std::uint_fast64_t>);
    std::random_device seed_gen;
    std::mt19937_64 engine(seed_gen());
    return uid {engine()};
#endif
  }

} // namespace yave