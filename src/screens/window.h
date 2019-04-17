#ifndef SRC_SCREENS_WINDOW_H_
#define SRC_SCREENS_WINDOW_H_

#include <ncurses.h>
#include <stdint.h>

#include "geometry.h"

class Window {
 public:
  uint32_t size_y, size_x;
  WINDOW *window;
  Window() {}
};

void window_close(Window *);

#endif  // SRC_SCREENS_WINDOW_H_
