#include "screens/window.h"

// delete window and re-render dungeon
void window_close(Window *win) {
  delwin(w);
  render_dungeon(d);
}
