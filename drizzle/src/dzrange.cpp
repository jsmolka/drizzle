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
  : DzIterator(iteree, "range"), iter(iteree->as<DzRange>()->start) {}

auto DzRangeIterator::done() const -> bool {
  const auto range = iteree->as<DzRange>();
  return range->step > 0
    ? iter >= range->stop
    : iter <= range->stop;
}

void DzRangeIterator::advance() {
  iter += iteree->as<DzRange>()->step;
}

auto DzRangeIterator::current(Gc&) const -> DzValue {
  return iter;
}

DzRangeReverseIterator::DzRangeReverseIterator(DzObject* iteree)
  : DzIterator(iteree, "range reverse") {
  const auto range = iteree->as<DzRange>();
  step = -range->step;
  stop =  range->start + step;
  iter =  range->stop + step;
}

auto DzRangeReverseIterator::done() const -> bool {
  return step > 0
    ? iter >= stop
    : iter <= stop;
}

void DzRangeReverseIterator::advance() {
  iter += step;
}

auto DzRangeReverseIterator::current(Gc&) const -> DzValue {
  return iter;
}
