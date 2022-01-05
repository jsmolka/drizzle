#pragma once

#include <string>
#include <string_view>
#include <vector>

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>
#include <sh/fmt.h>

using namespace boost::ut;

template<typename... Args>
auto test(std::string_view format, Args&&... args) -> detail::test {
  static std::vector<std::string> names;
  const auto& name = names.emplace_back(fmt::format(fmt::runtime(format), std::forward<Args>(args)...));
  return boost::ut::test(std::string_view(name.begin(), name.end()));
}
