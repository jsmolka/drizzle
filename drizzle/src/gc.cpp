#include "gc.h"

#include <sh/utility.h>

#include "dzboundmethod.h"
#include "dzfunction.h"
#include "dzinstance.h"
#include "dzlist.h"
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
  if (vm) {
    #ifndef DZ_FORCE_GC
    if (allocated > threshold) {
    #endif
      mark();
      sweep();
    #ifndef DZ_FORCE_GC
      threshold *= kGrowthFactor;
    }
    #endif
  }
}

void Gc::mark() {
  mark(vm->classes.list);
  for (const auto& value : vm->stack) {
    mark(value);
  }
  for (const auto& frame : vm->frames) {
    mark(frame.function);
  }
  for (const auto& global : vm->globals) {
    mark(global);
  }
}

void Gc::mark(const DzValue& value) {
  if (value.type == DzValue::Type::Object) {
    mark(value.o);
  }
}

void Gc::mark(DzObject* object) {
  static_assert(int(DzObject::Type::LastEnumValue) == 7);
  if (!object || object->marked) {
    return;
  }

  object->marked = true;
  switch (object->type) {
    case DzObject::Type::BoundMethod: {
      const auto method = static_cast<DzBoundMethod*>(object);
      mark(method->self);
      mark(method->function);
      break;
    }

    case DzObject::Type::Class: {
      const auto class_ = static_cast<DzClass*>(object);
      mark(class_->identifier);
      for (const auto& [key, value] : class_->functions) {
        mark(key);
        mark(value);
      }
      break;
    };

    case DzObject::Type::Function: {
      const auto function = static_cast<DzFunction*>(object);
      mark(function->identifier);
      for (const auto& [key, value] : function->identifiers) {
        mark(key);
      }
      if (function->isChunk()) {
        for (const auto& constant : function->chunk().constants) {
          mark(constant);
        }
      }
      break;
    }

    case DzObject::Type::Instance: {
      const auto instance = static_cast<DzInstance*>(object);
      mark(instance->class_);
      for (const auto& [key, value] : instance->fields) {
        mark(key);
        mark(value);
      }
      break;
    }

    case DzObject::Type::List: {
      const auto list = static_cast<DzList*>(object);
      mark(list->class_);
      for (const auto& value : list->values) {
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
