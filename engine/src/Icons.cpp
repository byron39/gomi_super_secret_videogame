#include "../include/Icons.hpp"

Icon::Icon(string path, i32 x, i32 y) {
  scale = {
      f32(x),
      f32(y),
      BOX_WIDTH,
      128,
  };
  texture = LoadTexture(path.c_str());

  text = path.substr(path.find_last_of('/') + 1,
                     path.size() - path.find_last_of('/') - 1);
  cout << text << endl;
  cout << scale.x << "  " << scale.y << "  " << scale.width << "  "
       << scale.height << endl;
  fontsize = 18;
  BorderThickness = 3;
}

void Icon::draw() {

  // this is kind of a hack but ehhh
  i32 charsPerLine = scale.width / fontsize;
  charsPerLine--;

  i32 lines = text.size() / charsPerLine;

  string drawText = text;

  for (i32 i = 0; i < lines; i++) {
    drawText.insert((i + 1) * charsPerLine, "\n");
  }
  DrawRectangleLinesEx(scale, BorderThickness, Color{30, 30, 30, 255});

  DrawText(TextFormat(drawText.c_str()), scale.x + fontsize, scale.y + fontsize,
           fontsize, BLACK);
  DrawTexturePro(texture, {0, 0, (f32)texture.width, (f32)texture.height},
                 Rectangle{scale.x + (BOX_WIDTH / 2), scale.y + BorderThickness,
                           f32((BOX_WIDTH / 2) - BorderThickness),
                           f32((scale.height) - 2 * BorderThickness)

                 },
                 {0, 0}, 0, RAYWHITE);
}
