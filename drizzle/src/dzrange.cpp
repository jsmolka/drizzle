#include "dzrange.h"

#include <sh/fmt.h>

#include "gc.h"
#include "vm.h"

DzRange::DzRange(dzint start, dzint stop, dzint step)
  : DzObject(Type::Range), start(start), stop(stop), step(step) {
  assert(step != 0);
}

auto DzRange::operator==(const DzObject& other) const -> bool {
  return other.type == Type::Range &&
    other.as<DzRange>()->start == start &&
    other.as<DzRange>()->stop == stop &&
    other.as<DzRange>()->step == step;
}

auto DzRange::repr() const -> std::string {
  return fmt::format("range({}, {}, {})", start, stop, step);
}

auto DzRange::makeIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzRangeIterator>(this);
}

auto DzRange::makeReverseIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzRangeReverseIterator>(this);
}

auto DzRange::in(Vm& vm, const DzValue& value) -> bool {
  vm.expect(value, DzValue::Type::Int);
  return step > 0
    ? value.i >= start && value.i < stop && (value.i - start) % step == 0
    : value.i <= start && value.i > stop && (value.i - start) % step == 0;
}

DzRangeIterator::DzRangeIterator(DzObject* iteree)
  : DzIterator(iteree), iter(iteree->as<DzRange>()->start) {}

auto DzRangeIterator::done() const -> bool {
  const auto range = iteree->as<DzRange>();
  return range->step > 0
    ? iter >= range->stop
    : iter <= range->stop;
}

void DzRangeIterator::advance() {
  iter += iteree->as<DzRange>()->step;
}

auto DzRangeIterator::value(Vm&) const -> DzValue {
  return iter;
}

DzRangeReverseIterator::DzRangeReverseIterator(DzObject* iteree)
  : DzIterator(iteree) {
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

auto DzRangeReverseIterator::value(Vm&) const -> DzValue {
  return iter;
}
