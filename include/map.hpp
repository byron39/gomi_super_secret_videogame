#pragma once
#include <raylib.h>

class Map {
public:
  Rectangle dst, src;
  Texture texture;

  Map();
  ~Map();
  void render();
  void update();
};
