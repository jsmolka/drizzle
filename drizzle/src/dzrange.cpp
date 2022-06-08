#include "dzrange.h"

#include <sh/fmt.h>

DzRange::DzRange(dzint start, dzint stop, dzint step)
  : DzObject(Type::Range), start(start), stop(stop), step(step) {
  assert(step != 0);
}

DzRange::operator bool() const {
  return true;
}

auto DzRange::operator==(const DzRange& other) const -> bool {
  return start == other.start && stop == other.stop && step == other.step;
}

auto DzRange::repr() const -> std::string {
  return fmt::format("range({}, {}, {})", start, stop, step);
}

DzRangeIterator::DzRangeIterator(DzObject* iteree)
  : DzIterator(iteree, "range") {
  set(iteree->as<DzRange>()->start);
}

void DzRangeIterator::advance() {
  set(value + iteree->as<DzRange>()->step);
}

auto DzRangeIterator::current(Gc&) const -> DzValue {
  return value;
}

void DzRangeIterator::set(dzint value) {
  this->value = value;
  const auto range = iteree->as<DzRange>();
  if (range->step > 0) {
    if (value >= range->stop) {
      iteree = nullptr;
    }
  } else {
    if (value <= range->stop) {
      iteree = nullptr;
    }
  }
}

DzRangeReverseIterator::DzRangeReverseIterator(DzObject* iteree)
  : DzIterator(iteree, "range reverse") {
  const auto range = iteree->as<DzRange>();
  step = -range->step;
  stop = range->start + step;
  set(range->stop + step);
}

void DzRangeReverseIterator::advance() {
  set(value + step);
}

auto DzRangeReverseIterator::current(Gc&) const -> DzValue {
  return value;
}

void DzRangeReverseIterator::set(dzint value) {
  this->value = value;
  if (step > 0) {
    if (value >= stop) {
      iteree = nullptr;
    }
  } else {
    if (value <= stop) {
      iteree = nullptr;
    }
  }
}
