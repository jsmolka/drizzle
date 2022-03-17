#pragma once

#include "dzobject.h"

class DzString : public DzObject {
public:
  struct Hash {
    auto operator()(const DzString* string) const -> std::size_t {
      return string->hash;
    }
  };

  struct Equal {
    auto operator()(const DzString* a, const DzString* b) const -> bool {
      return a->data == b->data;
    }
  };

  DzString(std::string_view data);
  DzString(const std::string& data);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string data;
  std::size_t hash;
};
