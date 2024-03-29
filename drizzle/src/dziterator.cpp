#include "dziterator.h"

#include "gc.h"

DzIterator::DzIterator(DzObject* iteree)
  : DzObject(Type::Iterator), iteree(iteree) {}

DzIterator::operator bool() const {
  if (iteree && done()) {
    iteree = nullptr;
  }
  return iteree;
}

void DzIterator::mark(Gc& gc) {
  DzObject::mark(gc);
  gc.mark(iteree);
}

auto DzIterator::makeIterator(Vm&) -> DzValue {
  return this;
}

DzSequenceIterator::DzSequenceIterator(DzObject* iteree)
  : DzIterator(iteree), index(0) {}

auto DzSequenceIterator::done() const -> bool {
  return index >= iteree->size();
}

void DzSequenceIterator::advance() {
  index++;
}

auto DzSequenceIterator::value(Vm& vm) const -> DzValue {
  return iteree->getItem(vm, index);
}

DzSequenceReverseIterator::DzSequenceReverseIterator(DzObject* iteree)
  : DzIterator(iteree), index(iteree->size() - 1) {}

auto DzSequenceReverseIterator::done() const -> bool {
  return index >= iteree->size();
}

void DzSequenceReverseIterator::advance() {
  index--;
}

auto DzSequenceReverseIterator::value(Vm& vm) const -> DzValue {
  return iteree->getItem(vm, index);
}
