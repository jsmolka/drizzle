#include "vm.h"

#include <chrono>
#include <thread>
#include <sh/args.h>
#include <sh/filesystem.h>
#include <sh/ranges.h>

#ifdef DZ_SDL
#  include "sdl2.h"
#endif

#include "dzbytes.h"
#include "dzfunction.h"
#include "dzlist.h"
#include "dznull.h"
#include "dzrange.h"
#include "dzwindow.h"
#include "gc.h"

namespace fs = sh::filesystem;

struct DzValuePrint : DzValue {};

template<>
struct fmt::formatter<DzValuePrint> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const DzValuePrint& value, FormatContext& ctx) const {
    return fmt::formatter<std::string>::format(
      value.isObject() && value->type == DzObject::Type::String
        ? value->as<DzString>()->data
        : value.repr(), ctx);
  }
};

void Vm::defineNatives() {
  const auto natives = {
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("arguments"), 0, [](Vm& vm, std::size_t) {
        const auto list = vm.gc.construct<DzList>();
        list->values.reserve(sh::args.size());
        for (const auto& argument : sh::args) {
          list->values.push_back(vm.gc.construct<DzString>(argument));
        }
        return list;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("assert"), 1, [](Vm& vm, std::size_t) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("bytes"), std::nullopt, [](Vm& vm, std::size_t argc) {
        const auto bytes = vm.gc.construct<DzBytes>();
        bytes->data.reserve(argc);
        for (const auto& value : sh::range(vm.stack.end() - argc, vm.stack.end())) {
          vm.expect(value, DzValue::Type::Int);
          bytes->data.push_back(value.i);
        }
        vm.stack.pop(argc);
        return bytes;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("len"), 1, [](Vm& vm, std::size_t) -> DzValue {
        auto value = vm.stack.pop_value();
        try {
          if (!value.isObject()) {
            throw NotSupportedException();
          }
          return static_cast<dzint>(value->size());
        } catch (const NotSupportedException&) {
          vm.raise("'{}' object has no len()", value.kind());
        }
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("forward"), 1, [](Vm& vm, std::size_t) {
        const auto iterator = vm.forward(vm.stack.top());
        vm.stack.pop();
        return iterator;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("print"), std::nullopt, [](Vm& vm, std::size_t argc) {
        fmt::print("{}\n", fmt::join(
          static_cast<const DzValuePrint*>(vm.stack.end()) - argc,
          static_cast<const DzValuePrint*>(vm.stack.end()), " "));
        vm.stack.pop(argc);
        return &null;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("range"), 3, [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        vm.expect(vm.stack.peek(2), DzValue::Type::Int);
        const auto step  = vm.stack.pop_value().i;
        const auto stop  = vm.stack.pop_value().i;
        const auto start = vm.stack.pop_value().i;
        if (step == 0) {
          vm.raise("range() step cannot be zero");
        }
        return vm.gc.construct<DzRange>(start, stop, step);
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("read_bin"), 1, [](Vm& vm, std::size_t) -> DzValue {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);
        const auto path = vm.stack.pop_value()->as<DzString>();
        const auto dest = vm.gc.construct<DzBytes>();
        if (fs::read(path->data, dest->data) != fs::status::ok) {
          return &null;
        }
        return dest;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("read_str"), 1, [](Vm& vm, std::size_t) -> DzValue {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);
        const auto path = vm.stack.pop_value()->as<DzString>();
        const auto dest = vm.gc.construct<DzString>();
        if (fs::read(path->data, dest->data) != fs::status::ok) {
          return &null;
        }
        return dest;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("repr"), 1, [](Vm& vm, std::size_t) {
        const auto string = vm.gc.construct<DzString>(vm.stack.top().repr());
        vm.stack.pop();
        return string;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("reverse"), 1, [](Vm& vm, std::size_t) {
        const auto iterator = vm.reverse(vm.stack.top());
        vm.stack.pop();
        return iterator;
      }
    ),
    #ifdef DZ_SDL
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("sdl_events"), 0, [](Vm& vm, std::size_t) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT) {
            return false;
          }
        }
        return true;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("sdl_keystate"), 1, [](Vm& vm, std::size_t) -> dzbool {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto key = vm.stack.pop_value().i;
        if (key >= 0 && key < SDL_NUM_SCANCODES) {
          return SDL_GetKeyboardState(NULL)[key];
        } else {
          return false;
        }
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("sdl_window"), 4, [](Vm& vm, std::size_t) -> DzValue {
        vm.expect(vm.stack.peek(3), DzObject::Type::String);
        vm.expect(vm.stack.peek(2), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);

        const auto window = vm.gc.construct<DzWindow>(
          vm.stack.peek(3)->as<DzString>(),
          vm.stack.peek(2).i,
          vm.stack.peek(1).i,
          vm.stack.peek(0).i
        );

        vm.stack.pop(4);
        if (!static_cast<bool>(*window)) {
          return &null;
        }
        return window;
      }
    ),
    #endif
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("sleep"), 1, [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto millis = vm.stack.pop_value().i;
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        return &null;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("time"), 0, [](Vm& vm, std::size_t) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("type"), 1, [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().kind());
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("write_bin"), 2, [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzObject::Type::Bytes);
        vm.expect(vm.stack.peek(1), DzObject::Type::String);
        const auto data = vm.stack.pop_value()->as<DzBytes>();
        const auto path = vm.stack.pop_value()->as<DzString>();
        return fs::write(path->data, data->data) == fs::status::ok;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("write_str"), 2, [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);
        vm.expect(vm.stack.peek(1), DzObject::Type::String);
        const auto data = vm.stack.pop_value()->as<DzString>();
        const auto path = vm.stack.pop_value()->as<DzString>();
        return fs::write(path->data, data->data) == fs::status::ok;
      }
    ),
  };

  for (const auto& native : natives) {
    for (const auto [index, global] : sh::enumerate(program.globals)) {
      if (native->identifier->data == global) {
        globals[index] = native;
        break;
      }
    }
  }
}
