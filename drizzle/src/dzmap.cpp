#include "dzmap.h"

#include "dznull.h"
#include "gc.h"
#include "vm.h"

using Pair = std::pair<DzValue, DzValue>;

template<>
struct fmt::formatter<Pair> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  };

  template<typename FormatContext>
  auto format(const Pair& pair, FormatContext& ctx) const {
    const auto& [key, value] = pair;
    return fmt::format_to(ctx.out(), "{}: {}", key, value);
  }
};

auto DzMap::Equal::operator()(const DzValue& a, const DzValue& b) const -> bool {
  return DzValue::binary(a, b, []<typename A, typename B>(const A& a, const B& b) {
    if constexpr (dz_primitive<A, B> || dz_object<A, B>) {
      return a == b;
    }
    return false;
  });
}

DzMap::DzMap()
  : DzObject(Type::Map) {}

DzMap::operator bool() const {
  return size() > 0;
}

auto DzMap::operator==(const DzObject& other) const -> bool {
  return other.type == Type::Map && other.as<DzMap>()->values == values;
}

auto DzMap::size() const -> std::size_t {
  return values.size();
}

auto DzMap::repr() const -> std::string {
  return fmt::format("{{{}}}", fmt::join(values, ", "));
}

void DzMap::mark(Gc& gc) {
  DzObject::mark(gc);
  for (const auto& [key, value] : values) {
    gc.mark(key);
    gc.mark(value);
  }
}

auto DzMap::in(Vm& vm, const DzValue& value) -> bool {
  return values.contains(value);
}

auto DzMap::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return values.get(expr).value_or(&null);
}

auto DzMap::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  defineMembers(vm);
  return DzObject::getProp(vm, prop, bind);
}

void DzMap::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  values.set(expr, value);
}

void DzMap::defineMembers(Vm& vm) {
  constexpr auto kSlot = int(Type::Map);
  if (vm.members[kSlot].size() > 0) {
    return;
  }

  const auto members = {
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("clear"), 0, [](Vm& vm, std::size_t) {
        const auto self = vm.stack.top()->as<DzMap>();
        self->values.clear();
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("get"), 1, [](Vm& vm, std::size_t) {
        const auto key  = vm.stack.pop_value();
        const auto self = vm.stack.top()->as<DzMap>();
        return self->values.get(key).value_or(&null);
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("set"), 2, [](Vm& vm, std::size_t) {
        const auto value = vm.stack.pop_value();
        const auto key   = vm.stack.pop_value();
        const auto self  = vm.stack.top()->as<DzMap>();
        self->values.set(key, value);
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("has"), 1, [](Vm& vm, std::size_t) {
        const auto key  = vm.stack.pop_value();
        const auto self = vm.stack.top()->as<DzMap>();
        return self->values.contains(key);
      }
    ),
  };

  for (const auto& member : members) {
    vm.members[kSlot].insert_or_assign(member->identifier, member);
  }
}
