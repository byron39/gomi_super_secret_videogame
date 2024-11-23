#include "../include/windowbar.hpp"
#include <raylib.h>

void Button::draw() {
  DrawRectanglePro(box, {0, 0}, 0, Color{200, 200, 200, 255});
  DrawRectangleLinesEx(box, 2, Color{170, 170, 170, 255});
  DrawText(text.c_str(), box.x + 5, box.y + 2, 12, BLACK);
}

void IconButton::construct() {
  this->text = "Icons";
  this->IsOpen = false;
  this->box = {MENU_BUTTON_WIDTH * 1, 0, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT};
}

void IconButton::onClick() { IsOpen = !IsOpen; }

void IconButton::draw() {
  DrawRectanglePro(box, {0, 0}, 0, Color{200, 200, 200, 255});
  DrawRectangleLinesEx(box, 2, Color{170, 170, 170, 255});

  if (IsOpen) {
    DrawRectanglePro({box.x + 1, box.y + 1, box.width - 2, box.height - 2},
                     {0, 0}, 0, {150, 200, 255, 255});
  }

  DrawText(text.c_str(), box.x + 5, box.y + 2, 16, BLACK);
}

void ShaderButton::construct() {
  text = "Shaders";
  IsOpen = false;
  box = {MENU_BUTTON_WIDTH * 2, 0, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT};
}

void ShaderButton::onClick() { IsOpen = !IsOpen; }

void ShaderButton::draw() {
  DrawRectanglePro(box, {0, 0}, 0, Color{200, 200, 200, 255});
  DrawRectangleLinesEx(box, 2, Color{170, 170, 170, 255});

  if (IsOpen) {
    DrawRectanglePro({box.x + 1, box.y + 1, box.width - 2, box.height - 2},
                     {0, 0}, 0, {150, 200, 255, 255});
  }

  DrawText(text.c_str(), box.x + 5, box.y + 2, 16, BLACK);
}

WindowBar::WindowBar() {
  barFrame = {0, 0, (f32)GetScreenWidth(), 21};
  icons = make_unique<IconButton>();
  shaders = make_unique<ShaderButton>();
  files = make_unique<FileButton>();
  Layers = make_unique<LayerButton>();

  icons->construct();
  shaders->construct();
  files->construct();
  Layers->construct();
}

void WindowBar::update() {
  Vector2 mouse = GetMousePosition();

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionPointRec(mouse, icons->box)) {
      icons->onClick();
      shaders->IsOpen = false;
    }
    if (CheckCollisionPointRec(mouse, shaders->box)) {
      shaders->onClick();
      icons->IsOpen = false;
    }
    if (CheckCollisionPointRec(mouse, files->box)) {
      files->onClick();
    }
    if (CheckCollisionPointRec(mouse, Layers->box)) {
      Layers->onClick();
    }
  }
}

void WindowBar::draw() {
  Layers->draw();
  icons->draw();
  shaders->draw();
  files->draw();
}

void FileButton::draw() {
  DrawRectanglePro(box, {0, 0}, 0, Color{200, 200, 200, 255});
  DrawRectangleLinesEx(box, 2, Color{170, 170, 170, 255});
  if (IsOpen) {
    DrawRectanglePro({box.x + 1, box.y + 1, box.width - 2, box.height - 2},
                     {0, 0}, 0, {150, 200, 255, 255});
  }

  DrawText(text.c_str(), box.x + 5, box.y + 2, 16, BLACK);
}

void FileButton::construct() {
  box = {0, 0, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT};
  text = "File";
}

void FileButton::onClick() { IsOpen = !IsOpen; }

void FileButton::FormatSaveData() {}

void LayerButton::onClick() { IsOpen = !IsOpen; }

void LayerButton::draw() {
  DrawRectanglePro(box, {0, 0}, 0, Color{200, 200, 200, 255});
  DrawRectangleLinesEx(box, 2, Color{170, 170, 170, 255});
  if (IsOpen) {
    DrawRectanglePro({box.x + 1, box.y + 1, box.width - 2, box.height - 2},
                     {0, 0}, 0, {150, 200, 255, 255});
  }

  DrawText(text.c_str(), box.x + 5, box.y + 2, 16, BLACK);
}

void LayerButton::construct() {
  box = {MENU_BUTTON_WIDTH * 3, 0, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT};
  text = "Layers";
  IsOpen = false;
}
