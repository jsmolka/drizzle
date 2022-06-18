#pragma once

#include <string>
#include <sh/vector.h>

class Arguments : public sh::vector<std::string> {
public:
  Arguments() = default;
  Arguments(int argc, const char* const* argv)
    : sh::vector<std::string>(argv, argv + argc) {}
};
