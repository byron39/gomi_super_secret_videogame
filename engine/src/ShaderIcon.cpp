#include "../include/ShaderIcon.hpp"
#include <raylib.h>

ShaderIcon::ShaderIcon(string &path, i32 x, i32 y) {
  scale = {
      f32(x),
      f32(y),
      BOX_WIDTH,
      128,
  };

  shader = LoadShader(0, path.c_str());

  // just realized this wont be portable for windows
  text = path.substr(path.find_last_of('/') + 1,
                     path.size() - path.find_last_of('/') - 1);
  fontsize = 18;
  BorderThickness = 3;
}
void ShaderIcon::draw(Rectangle *barFrame) {

  // this is kind of a hack but ehhh
  i32 charsPerLine = scale.width / fontsize;
  charsPerLine--;
  Rectangle dest = {scale.x, scale.y + (barFrame->height * 2),
                    f32((BOX_WIDTH)-BorderThickness),
                    f32((scale.height) - 2 * BorderThickness)};

  i32 lines = text.size() / charsPerLine;

  string drawText = text;

  for (i32 i = 0; i < lines; i++) {
    drawText.insert((i + 1) * charsPerLine, "\n");
  }
  DrawRectangleLinesEx(dest, BorderThickness, Color{30, 30, 30, 255});

  DrawText(TextFormat(drawText.c_str()), dest.x + fontsize, dest.y + fontsize,
           fontsize, BLACK);
}
