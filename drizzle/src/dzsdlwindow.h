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

class DzSdlWindow : public DzObject {
public:
  DzSdlWindow(DzString* title, dzint w, dzint h, dzint scale);
  ~DzSdlWindow();

  operator bool() const;
  auto repr() const -> std::string;

  void dispose();

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
  std::vector<u32> buffer;
};
