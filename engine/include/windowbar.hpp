#pragma once
#include "list.hpp"
#include "toml.hpp"
#include "types.hpp"
#include <memory>
#include <raylib.h>

#define MENU_BUTTON_WIDTH 128
#define MENU_BUTTON_HEIGHT 40

class Button {
public:
  Rectangle box;
  string text;
  virtual void draw();        // this one gets a default
  virtual void construct() {} // this is a constructor, don't question me
  virtual void onClick() {}
};

class FileButton : public Button {
public:
  bool IsOpen;
  void FormatSaveData(); // this will be a painful implementation
  void construct() override;
  void onClick() override;
  void draw() override;
};
class IconButton : public Button {
public:
  bool IsOpen;
  void construct() override;
  void onClick() override;
  void draw() override;
};
class ShaderButton : public Button {
public:
  bool IsOpen;
  void construct() override;
  void onClick() override;
  void draw() override;
};
class LayerButton : public Button {
public:
  bool IsOpen;
  void construct() override;
  void onClick() override;
  void draw() override;
};

class WindowBar {

public:
  Rectangle barFrame;

  unique_ptr<IconButton> icons;
  unique_ptr<ShaderButton> shaders;
  unique_ptr<FileButton> files;
  unique_ptr<LayerButton> Layers;

  WindowBar();
  void draw();
  void update();
};
