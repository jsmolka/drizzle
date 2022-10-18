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

class DzWindow : public DzObject {
public:
  DzWindow(DzString* title, dzint w, dzint h, dzint scale);
  ~DzWindow();

  virtual operator bool() const override;
  virtual auto repr() const -> std::string override;

  void dispose();

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
  std::vector<u32> buffer;
};
