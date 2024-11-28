#pragma once
#include "list.hpp"
#include "selectable.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <raylib.h>

#define BOX_WIDTH 256

class TextureIcon : public Selectable {
public:
  Rectangle scale;
  string text;
  i32 fontsize;
  Texture texture;
  i32 border_thickness;
  node<TextureIcon *> *ref;

  void draw(Rectangle *barFrame);

  TextureIcon(string path, i32 x, i32 y);
  void make_ref(node<TextureIcon *> *reference);
};
