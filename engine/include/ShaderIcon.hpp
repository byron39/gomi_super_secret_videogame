#pragma once

#include "Icons.hpp"
#include "selectable.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <raylib.h>

class ShaderIcon : Selectable {
public:
  Shader shader;
  Rectangle scale;
  string text;
  i32 fontsize;
  i32 BorderThickness;
  node<ShaderIcon *> *ref;

  ShaderIcon(string &path, i32 x, i32 y);
  void make_ref(node<ShaderIcon *> reference);
  void draw(Rectangle *barFrame);
};
