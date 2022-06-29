#include "vm.h"

#include "dznull.h"
#include "dzsdlwindow.h"
#include "gc.h"

#if DZ_SDL
#  include "sdl2.h"
#endif

void Vm::defineSdlWindowMembers() {
  #if DZ_SDL
  const auto members = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("set_title"), Arity::equal(1), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzObject::Type::String);

        const auto title  = vm.stack.pop_value().o->as<DzString>();
        const auto window = vm.stack.top().o->as<DzSdlWindow>();

        SDL_SetWindowTitle(window->window, title->data.c_str());
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("set_pixel"), Arity::equal(3), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        vm.expect(vm.stack.peek(2), DzValue::Type::Int);

        const auto color  = vm.stack.pop_value().i;
        const auto y      = vm.stack.pop_value().i;
        const auto x      = vm.stack.pop_value().i;
        const auto window = vm.stack.top().o->as<DzSdlWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);

        if (x < 0 || x >= w || y < 0 || y >= h) {
          vm.raise("pixel index out of range");
        }
        window->buffer[w * y + x] = static_cast<u32>(color);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("width"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto window = vm.stack.top().o->as<DzSdlWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);
        return w;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("height"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto window = vm.stack.top().o->as<DzSdlWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);
        return h;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("clear"), Arity::equal(1), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);

        const auto color  = vm.stack.pop_value().i;
        const auto window = vm.stack.top().o->as<DzSdlWindow>();
        std::fill(window->buffer.begin(), window->buffer.end(), static_cast<u32>(color));
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("render"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto window = vm.stack.top().o->as<DzSdlWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);

        SDL_UpdateTexture(window->texture, NULL, window->buffer.data(), sizeof(u32) * w);
        SDL_RenderCopy(window->renderer, window->texture, NULL, NULL);
        SDL_RenderPresent(window->renderer);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("dispose"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto window = vm.stack.top().o->as<DzSdlWindow>();
        window->dispose();
        return &null;
      }
    ),
  };

  for (const auto& member : members) {
    this->members[int(DzObject::Type::SdlWindow)].insert_or_assign(member->identifier, member);
  }
  #endif
}

