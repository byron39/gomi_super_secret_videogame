#pragma once
#include "list.hpp"
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
  node<Icon *> *ref;

  void draw();

  Icon(string path, i32 x, i32 y);
  void make_ref(node<Icon *> *reference);
};
