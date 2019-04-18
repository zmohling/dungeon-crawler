#ifndef SCREENS_WINDOW_H_
#define SCREENS_WINDOW_H_

#include <ncurses.h>
#include <stdint.h>
#include <string>

#include "dungeon.h"
#include "geometry.h"

// Custom curses window class to facilitate and ease popup screens
class Window {
 private:
  void init_win();
  void init_texts();
  void scroll_down();
  void scroll_up();

 public:
  uint32_t size_y, size_x;
  point_t start;
  WINDOW *window;

  std::string title, footer = "PRESS ESC TO EXIT", heading;

  Window(uint32_t size_y, uint32_t size_x) {
    this->size_y = size_y;
    this->size_x = size_x;

    start.y = ((DUNGEON_Y - size_y) / 2) + 1;
    start.x = (DUNGEON_X - size_x) / 2;
  }

  Window(uint32_t size_y, uint32_t size_x, point_t start) {
    this->size_y = size_y;
    this->size_x = size_x;
    this->start = start;
  }

  Window(point_t start, point_t end) {
    this->size_y = start.y - end.y;
    this->size_x = start.x - end.x;
    this->start = start;
  }

  void open();
  void close();

  void set_title(const std::string title) { this->title = title; }
  void set_footer(const std::string footer) { this->footer = footer; }
  void set_heading(const std::string heading) { this->heading = heading; }
};

#endif  // SCREENS_WINDOW_H_
