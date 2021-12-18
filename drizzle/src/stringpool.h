#pragma once

#include "dzmap.h"
#include "dzstring.h"

class StringPool {
 public:
  ~StringPool();

  auto make(std::string data) -> DzString*;

 private:
  DzMap<DzString*> pool;
};
