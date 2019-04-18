#ifndef SCREENS_INVENTORY_H_
#define SCREENS_INVENTORY_H_

#include <string>

#include "character.h"
#include "geometry.h"
#include "screens/window.h"

class InventoryWindow : public Window {
 private:
  pc *c = pc::instance();
  std::string title = "INVENTORY", footer = "PRESS ESC";

  point_t start;

 public:
  // InventoryWindow() :
};

#endif  // SCREENS_INVENTORY_H_
