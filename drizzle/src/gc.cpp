#include "gc.h"

#include <sh/utility.h>

#include "dzboundmethod.h"
#include "dzfunction.h"
#include "dzinstance.h"
#include "dziterator.h"
#include "dzlist.h"
#include "dzmap.h"
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
  if (value.type == DzValue::Type::Object) {
    mark(value.o);
  }
}

void Gc::mark(DzObject* object) {
  static_assert(int(DzObject::Type::LastEnumValue) == 11);
  if (!object || object->marked) {
    return;
  }

  object->marked = true;
  switch (object->type) {
    case DzObject::Type::BoundMethod: {
      const auto method = object->as<DzBoundMethod>();
      mark(method->self);
      mark(method->function);
      break;
    }
    case DzObject::Type::Class: {
      const auto class_ = object->as<DzClass>();
      mark(class_->identifier);
      for (const auto& [key, value] : class_->functions) {
        mark(key);
        mark(value);
      }
      break;
    };
    case DzObject::Type::Function: {
      const auto function = object->as<DzFunction>();
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
      const auto instance = object->as<DzInstance>();
      mark(instance->class_);
      for (const auto& [key, value] : instance->fields) {
        mark(key);
        mark(value);
      }
      break;
    }
    case DzObject::Type::Iterator: {
      const auto iterator = object->as<DzIterator>();
      mark(iterator->iteree);
      break;
    }
    case DzObject::Type::List: {
      const auto list = object->as<DzList>();
      for (const auto& value : list->values) {
        mark(value);
      }
      break;
    }
    case DzObject::Type::Map: {
      const auto map = object->as<DzMap>();
      for (const auto& [key, value] : map->values) {
        mark(key);
        mark(value);
      }
      break;
    }
    case DzObject::Type::ReverseIterator: {
      const auto iterator = object->as<DzReverseIterator>();
      mark(iterator->iteree);
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
