#pragma once

#include "list.hpp"
#include "types.hpp"
#include <raylib.h>

class Arrow {
  node<Arrow *> key_node; // used for removal
public:
  Arrow();
  void render(Texture *text);
};

class ArrowManager {
  unique_ptr<DLinkedList<Arrow *>> arrows;
  Texture base_texture;

public:
  ArrowManager();
  void add_new(Rectangle player_pos);
  void render();
};
