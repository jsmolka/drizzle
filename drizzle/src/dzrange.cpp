#include "dzrange.h"

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
  return vm.gc.construct<DzRangeIterator>(this, start, stop, step);
}

auto DzRange::makeReverseIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzRangeIterator>(this, stop - step, start - step, -step);
}

auto DzRange::in(Vm& vm, const DzValue& value) -> bool {
  vm.expect(value, DzValue::Type::Int);
  return step > 0
    ? value.i >= start && value.i < stop && (value.i - start) % step == 0
    : value.i <= start && value.i > stop && (value.i - start) % step == 0;
}

DzRangeIterator::DzRangeIterator(DzRange* iteree, dzint iter, dzint stop, dzint step)
  : DzIterator(iteree), iter(iter), stop(stop), step(step) {}

auto DzRangeIterator::done() const -> bool {
  return step > 0
    ? iter >= stop
    : iter <= stop;
}

void DzRangeIterator::advance() {
  iter += step;
}

auto DzRangeIterator::value(Vm&) const -> DzValue {
  return iter;
}
