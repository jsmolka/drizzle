#include "gc.h"

#include "vm.h"

Gc::~Gc() {
  auto head = objects;
  while (head) {
    auto temp = head;
    head = head->next;
    delete temp;
  }
}

void Gc::collect() {
  #ifdef DZ_FORCE_GC
  mark();
  sweep();
  #else
  if (allocated > threshold) {
    mark();
    sweep();
    threshold *= kGrowthFactor;
  }
  #endif
}

void Gc::mark() {
  for (const auto& value : vm->stack) {
    mark(value);
  }
  for (const auto& frame : vm->frames) {
    mark(frame.function);
  }
  for (const auto& global : vm->globals) {
    mark(global);
  }
  for (const auto& members : vm->members) {
    for (const auto& [key, value] : members) {
      mark(key);
      mark(value);
    }
  }
}

void Gc::mark(const DzValue& value) {
  if (value.isObject()) {
    mark(value.o);
  }
}

void Gc::mark(DzObject* object) {
  if (object && !object->marked) {
    object->mark(*this);
  }
}

void Gc::sweep() {
  for (auto iter = pool.begin(); iter != pool.end(); ) {
    if ((*iter)->marked) {
      iter++;
    } else {
      iter = pool.erase(iter);
    }
  }

  auto head = objects;
  auto prev = static_cast<DzObject*>(nullptr);
  while (head) {
    if (head->marked) {
      head->marked = false;
      prev = head;
      head = head->next;
    } else {
      auto temp = head;
      head = head->next;
      if (prev) {
        prev->next = head;
      } else {
        objects = head;
      }
      delete temp;
    }
  }
}
