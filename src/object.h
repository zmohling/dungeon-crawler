#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#include <stdint.h>
#include <string>

#include "descriptions.h"
#include "dice.h"
#include "geometry.h"

class object {
 private:
 public:
  std::string name, description;
  object_type_t type;
  uint32_t color, hit, dodge, defence, weight, speed, attribute, value, rarity;
  dice damage;
  bool artifact;
  point_t position;

  std::ostream &print(std::ostream &o);
};

std::ostream &operator<<(std::ostream &o, monster_description &m);

#endif  // SRC_OBJECT_H_
