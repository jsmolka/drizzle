#include "vm.h"

#include <chrono>
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
#include "dzsdlwindow.h"
#include "gc.h"

namespace fs = sh::filesystem;

struct DzValuePrint : DzValue {};

template<>
struct fmt::formatter<DzValuePrint> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const DzValuePrint& value, FormatContext& ctx) const {
    return fmt::formatter<std::string>::format(
      value.is(DzObject::Type::String)
        ? value.o->as<DzString>()->data
        : value.repr(), ctx);
  }
};

void Vm::defineNatives() {
  const auto natives = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("arguments"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto list = vm.gc.construct<DzList>();
        list->values.reserve(vm.arguments.size());
        for (const auto& argument : vm.arguments) {
          list->values.push_back(vm.gc.construct<DzString>(argument));
        }
        return list;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("assert"), Arity::equal(1), [](Vm& vm, std::size_t) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("bytes"), Arity::greaterEqual(0), [](Vm& vm, std::size_t argc) {
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
    gc.construct<DzFunction>(
      gc.construct<DzString>("forward"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto iterator = vm.forward(vm.stack.top());
        vm.stack.pop();
        return iterator;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("print"), Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) {
        fmt::print("{}\n", fmt::join(
          static_cast<DzValuePrint*>(vm.stack.end()) - argc,
          static_cast<DzValuePrint*>(vm.stack.end()), " "));
        vm.stack.pop(argc);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("range"), Arity::equal(3), [](Vm& vm, std::size_t) {
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
    gc.construct<DzFunction>(
      gc.construct<DzString>("read_bin"), Arity::equal(1), [](Vm& vm, std::size_t) -> DzValue {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);
        const auto path = vm.stack.pop_value().o->as<DzString>();
        const auto dest = vm.gc.construct<DzBytes>();
        if (fs::read(path->data, dest->data) != fs::status::ok) {
          return &null;
        }
        return dest;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("read_str"), Arity::equal(1), [](Vm& vm, std::size_t) -> DzValue {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);
        const auto path = vm.stack.pop_value().o->as<DzString>();
        const auto dest = vm.gc.construct<DzString>();
        if (fs::read(path->data, dest->data) != fs::status::ok) {
          return &null;
        }
        return dest;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("reverse"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto iterator = vm.reverse(vm.stack.top());
        vm.stack.pop();
        return iterator;
      }
    ),
    #ifdef DZ_SDL
    gc.construct<DzFunction>(
      gc.construct<DzString>("sdl_events"), Arity::equal(0), [](Vm& vm, std::size_t) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT) {
            return false;
          }
        }
        return true;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("sdl_keystate"), Arity::equal(1), [](Vm& vm, std::size_t) -> dzbool {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto key = vm.stack.pop_value().i;
        if (key >= 0 && key < SDL_NUM_SCANCODES) {
          return SDL_GetKeyboardState(NULL)[key];
        } else {
          return false;
        }
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("sdl_window"), Arity::equal(4), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(3), DzObject::Type::String);
        vm.expect(vm.stack.peek(2), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);

        const auto window = vm.gc.construct<DzSdlWindow>();
        const auto scale  = vm.stack.pop_value().i;
        const auto h      = vm.stack.pop_value().i;
        const auto w      = vm.stack.pop_value().i;
        const auto title  = vm.stack.pop_value().o->as<DzString>();

        window->window = SDL_CreateWindow(
          title->data.c_str(),
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          w * scale,
          h * scale,
          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );
        return window;
      }
    ),
    #endif
    gc.construct<DzFunction>(
      gc.construct<DzString>("time"), Arity::equal(0), [](Vm& vm, std::size_t) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("type"), Arity::equal(1), [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().kind());
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("write_bin"), Arity::equal(2), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzObject::Type::Bytes);
        vm.expect(vm.stack.peek(1), DzObject::Type::String);
        const auto data = vm.stack.pop_value().o->as<DzBytes>();
        const auto path = vm.stack.pop_value().o->as<DzString>();
        return fs::write(path->data, data->data) == fs::status::ok;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("write_str"), Arity::equal(2), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);
        vm.expect(vm.stack.peek(1), DzObject::Type::String);
        const auto data = vm.stack.pop_value().o->as<DzString>();
        const auto path = vm.stack.pop_value().o->as<DzString>();
        return fs::write(path->data, data->data) == fs::status::ok;
      }
    ),
  };

  const auto main = frames[0].function;
  for (const auto& native : natives) {
    const auto iter = main->identifiers.find(native->identifier);
    if (iter != main->identifiers.end()) {
      const auto& [identifier, index] = *iter;
      globals[index] = native;
    }
  }
}
