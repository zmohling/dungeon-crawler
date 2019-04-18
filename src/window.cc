#include "screens/window.h"

void Window::init_win() {
  this->window = newwin(size_y, size_x, start.y, start.x);
  werase(this->window);
  box(this->window, 0, 0);

  wrefresh(this->window);
  overwrite(this->window, stdscr);
}

void Window::init_texts() {
  mvprintw(start.y, start.x + (size_x / 2) - (title.length() / 2),
           title.c_str());
  mvprintw(start.y + size_y - 1, start.x + (size_x / 2) - (footer.length() / 2),
           footer.c_str());

  if (!heading.empty()) {
    mvprintw(start.y + 1, start.x + (size_x / 2) - (heading.length() / 2),
             heading.c_str());
  }
}

void Window::open() {
  if (title.empty() || footer.empty()) {
    throw "Title or Footer was not set";
  } else {
    init_win();
    init_texts();
  }
}

void Window::close() {
  delwin(this->window);
  render_dungeon(dungeon_t::instance());
}
