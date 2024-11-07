#include "../include/arrows.hpp"
#include <memory>
#include <raylib.h>

Arrow::Arrow() {}
ArrowManager::ArrowManager() {
  arrows = std::make_unique<DLinkedList<Arrow *>>();
}

void ArrowManager::add_new(Rectangle player_pos) {
  arrows->append(new Arrow());
}
