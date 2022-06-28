#pragma once

#include "dzobject.h"

class SDL_Window;

class DzSdlWindow : public DzObject {
public:
  DzSdlWindow(SDL_Window* window);

  operator bool() const;
  auto repr() const -> std::string;

  SDL_Window* window;
};
