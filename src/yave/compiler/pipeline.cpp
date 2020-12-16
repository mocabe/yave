//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/pipeline.hpp>
#include <yave/support/log.hpp>

#include <map>

YAVE_DECL_LOCAL_LOGGER(compiler_pipeline)

namespace yave::compiler {

  class pipeline::impl
  {
    /// success?
    bool m_success = true;
    /// values
    std::map<std::string, unique_any> m_data;

  public:
    impl()
    {
    }

  public:
    void add_data_any(const std::string& key, unique_any val)
    {
      auto [it, succ] = m_data.try_emplace(key, std::move(val));
      (void)it;

      if (!succ) {
        log_warning( "Could not add data: already exists");
      }
    }

    void remove_data_any(const std::string& key)
    {
      m_data.erase(key);
    }

    auto get_data_any(const std::string& key) -> unique_any*
    {
      auto it = m_data.find(key);

      if (it == m_data.end())
        return nullptr;

      return &it->second;
    }

  public:
    bool success() const
    {
      return m_success;
    }

    void set_failed()
    {
      m_success = false;
    }
  };

  pipeline::pipeline()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  pipeline::~pipeline() noexcept = default;

  pipeline::pipeline(pipeline&&) noexcept = default;

  void pipeline::_add_data_any(const std::string& key, unique_any val)
  {
    m_pimpl->add_data_any(key, std::move(val));
  }

  void pipeline::_remove_data_any(const std::string& key)
  {
    m_pimpl->remove_data_any(key);
  }

  auto pipeline::_get_data_any(const std::string& key) -> unique_any*
  {
    return m_pimpl->get_data_any(key);
  }

  bool pipeline::success() const
  {
    return m_pimpl->success();
  }

  void pipeline::set_failed()
  {
    m_pimpl->set_failed();
  }

} // namespace yave::compiler