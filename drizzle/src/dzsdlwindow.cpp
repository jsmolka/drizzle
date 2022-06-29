#include "dzsdlwindow.h"

#include <sh/fmt.h>
#include <sh/utility.h>

#ifdef DZ_SDL
#  include "sdl2.h"
#endif

DzSdlWindow::DzSdlWindow(DzString* title, dzint w, dzint h, dzint scale)
  : DzObject(Type::SdlWindow) {
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
    SDL_WINDOW_RESIZABLE);

  if (!window) {
    return;
  }

  renderer = SDL_CreateRenderer(
    window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  if (!renderer) {
    return;
  }

  SDL_RenderSetLogicalSize(renderer, w, h);

  texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    w, h);

  buffer.resize(w * h, 0xFFFFFFFF);
  #endif
}

DzSdlWindow::~DzSdlWindow() {
  dispose();
}

DzSdlWindow::operator bool() const {
  return window && renderer && texture;
}

auto DzSdlWindow::repr() const -> std::string {
  return fmt::format("<SDL window instance at 0x{:016X}>", sh::cast<sh::u64>(this));
}

void DzSdlWindow::dispose() {
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
