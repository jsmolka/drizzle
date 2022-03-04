#include "gc.h"

#include <sh/utility.h>

#include "vm.h"

Gc::~Gc() {
  auto object = objects;
  while (object) {
    auto next = object->next;
    delete object;
    object = next;
  }
}

void Gc::collect() {
  fmt::print("-- gc\n");
  mark();
  trace();
  sweep();
  fmt::print("-- /gc\n");
}

void Gc::mark() {
  assert(vm);
  for (auto& value : vm->stack) {
    mark(value);
  }
}

void Gc::mark(DzValue& value) {
  if (value.type == DzValue::Type::Object) {
    mark(value.o);
  }
}

void Gc::mark(DzObject* object) {
  if (object && !object->marked) {
    fmt::print("mark {}\n", object->repr());
    object->marked = true;
    gray.push_back(object);
  }
}

void Gc::trace() {
  for (auto object : gray) {
    blacken(object);
  }
  gray.clear();
}

void Gc::blacken(DzObject* object) {
  fmt::print("blacken {}\n", object->repr());

  switch (object->type) {
    case DzObject::Type::Function: {
      auto function = static_cast<DzFunction*>(object);
      for (auto& value : function->chunk.constants) {
        mark(value);
      }
      break;
    }
  }
}

void Gc::sweep() {
  DzObject* object = objects;
  DzObject* previous = nullptr;
  while (object) {
    if (object->marked) {
      object->marked = false;
      previous = object;
      object = object->next;
    } else {
      auto unreached = object;
      object = object->next;
      if (previous) {
        previous->next = object;
      } else {
        objects = object;
      }
      fmt::print("sweep {}\n", unreached->repr());
      delete unreached;
    }
  }
}
