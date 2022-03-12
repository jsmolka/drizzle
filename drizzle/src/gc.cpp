#include "gc.h"

#include <sh/utility.h>

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
  if (vm && allocated > threshold) {
    mark();
    sweep();
    threshold *= kGrowthFactor;
  }
}

void Gc::mark() {
  assert(vm);
  for (auto& value : vm->stack) {
    mark(value);
  }
  for (auto& frame : vm->frames) {
    mark(frame.function);
  }
  mark(vm->frame.function);
}

void Gc::mark(DzValue& value) {
  if (value.type == DzValue::Type::Object) {
    mark(value.o);
  }
}

void Gc::mark(DzObject* object) {
  static_assert(int(DzObject::Type::LastEnumValue) == 5);
  assert(object);
  if (object->marked) {
    return;
  }

  object->marked = true;
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
