#include "dzsdlwindow.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzSdlWindow::DzSdlWindow(SDL_Window* window)
  : DzObject(Type::SdlWindow), window(window) {}

DzSdlWindow::operator bool() const {
  return window;
}

auto DzSdlWindow::repr() const -> std::string {
  return fmt::format("<SDL window instance at 0x{:016X}>", sh::cast<sh::u64>(this));
}
