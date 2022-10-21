#pragma once

#include <vector>

#include "dzobject.h"
#include "dzstring.h"

struct SDL_Window
#ifndef DZ_SDL
{}
#endif
;

struct SDL_Renderer
#ifndef DZ_SDL
{}
#endif
;

struct SDL_Texture
#ifndef DZ_SDL
{}
#endif
;

class DzWindow final : public DzObject {
public:
  DzWindow(const DzString* title, dzint w, dzint h, dzint scale);
  ~DzWindow();

  explicit operator bool() const override final;

  auto getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue override final;

private:
  static void members(Vm& vm);

  void dispose();

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
  std::vector<u32> buffer;
};
