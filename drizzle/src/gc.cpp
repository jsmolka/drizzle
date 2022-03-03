#include "gc.h"

Gc::~Gc() {
  auto object = objects;
  while (object) {
    auto next = object->next;
    delete object;
    object = next;
  }
}
