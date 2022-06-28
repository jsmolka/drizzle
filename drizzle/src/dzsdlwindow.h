#pragma once

#include "dzobject.h"

struct SDL_Window
#ifndef DZ_SDL
{}
#endif
;

class DzSdlWindow : public DzObject {
public:
  DzSdlWindow();

  operator bool() const;
  auto repr() const -> std::string;

  SDL_Window* window = nullptr;
};
