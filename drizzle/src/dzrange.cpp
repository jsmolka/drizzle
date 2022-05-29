#include "dzrange.h"

#include <sh/fmt.h>

DzRange::DzRange(dzint start, dzint stop, dzint step)
  : DzObject(Type::Range), start(start), stop(stop), step(step) {}

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

auto DzRangeIterator::dereference(Gc&) const -> DzValue {
  return value;
}

void DzRangeIterator::set(dzint value) {
  this->value = value;
  const auto range = iteree->as<DzRange>();
  if (range->step > 0) {
    if (value >= range->stop) {
      iteree = nullptr;
    }
  } else if (range->step < 0) {
    if (value <= range->stop) {
      iteree = nullptr;
    }
  }
}
