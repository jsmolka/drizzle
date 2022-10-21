#include "dzwindow.h"

#ifdef DZ_SDL
#  include "sdl2.h"
#endif

#include "dznull.h"
#include "gc.h"
#include "vm.h"

DzWindow::DzWindow(const DzString* title, dzint w, dzint h, dzint scale)
  : DzObject(Type::Window) {
  #ifdef DZ_SDL
  if (!SDL_WasInit(SDL_INIT_VIDEO)) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
      return;
    }
  }

  if (w < 0 || h < 0 || scale < 1) {
    return;
  }

  window = SDL_CreateWindow(
    title->data.c_str(),
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    w * scale,
    h * scale,
    SDL_WINDOW_RESIZABLE
  );

  if (!window) {
    return;
  }

  renderer = SDL_CreateRenderer(
    window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
  );

  if (!renderer) {
    return;
  }

  SDL_RenderSetLogicalSize(renderer, w, h);

  texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    w, h
  );

  buffer.resize(w * h, 0xFFFFFFFF);
  #endif
}

DzWindow::~DzWindow() {
  dispose();
}

DzWindow::operator bool() const {
  return window && renderer && texture;
}

auto DzWindow::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  members(vm);
  return DzObject::getProp(vm, prop, bind);
}

void DzWindow::members(Vm& vm) {
  #if DZ_SDL
  constexpr auto kSlot = int(Type::Window);
  if (vm.members[kSlot].size() > 0) {
    return;
  }

  const auto members = {
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("title"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto title  = vm.stack.pop_value();
        const auto window = vm.stack.top()->as<DzWindow>();
        vm.expect(title, DzObject::Type::String);
        SDL_SetWindowTitle(window->window, title->as<DzString>()->data.c_str());
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("pixel"), Arity::equal(3), [](Vm& vm, std::size_t) {
        const auto color  = vm.stack.pop_value();
        const auto y      = vm.stack.pop_value();
        const auto x      = vm.stack.pop_value();
        const auto window = vm.stack.top()->as<DzWindow>();

        vm.expect(color, DzValue::Type::Int);
        vm.expect(x,     DzValue::Type::Int);
        vm.expect(y,     DzValue::Type::Int);

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);

        if (x.i < 0 || x.i >= w || y.i < 0 || y.i >= h) {
          vm.raise("pixel index out of range");
        }
        window->buffer[w * y.i + x.i] = static_cast<u32>(color.i);
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("width"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto window = vm.stack.top()->as<DzWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);
        return w;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("height"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto window = vm.stack.top()->as<DzWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);
        return h;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("clear"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto color  = vm.stack.pop_value();
        const auto window = vm.stack.top()->as<DzWindow>();
        vm.expect(color, DzValue::Type::Int);
        std::fill(window->buffer.begin(), window->buffer.end(), static_cast<u32>(color.i));
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("render"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto window = vm.stack.top()->as<DzWindow>();

        int w;
        int h;
        SDL_RenderGetLogicalSize(window->renderer, &w, &h);

        SDL_UpdateTexture(window->texture, NULL, window->buffer.data(), sizeof(u32) * w);
        SDL_RenderCopy(window->renderer, window->texture, NULL, NULL);
        SDL_RenderPresent(window->renderer);
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("dispose"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto window = vm.stack.top()->as<DzWindow>();
        window->dispose();
        return &null;
      }
    ),
  };

  for (const auto& member : members) {
    vm.members[kSlot].insert_or_assign(member->identifier, member);
  }
  #endif
}

void DzWindow::dispose() {
  #ifdef DZ_SDL
  if (texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  #endif
}
