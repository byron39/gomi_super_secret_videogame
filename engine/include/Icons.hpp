#pragma once
#include "types.hpp"
#include <raylib.h>

#define BOX_WIDTH 512

class Icon {
public:
  Rectangle scale;
  string text;
  i32 fontsize;
  Texture texture;
  i32 BorderThickness;
  Icon(string path, i32 x, i32 y);

  void draw();
};
