#pragma once

#include "GameObjects.hpp"
#include "Icons.hpp"
#include "types.hpp"
#include <raylib.h>

class GameObjectContainer {
  vector<DLinkedList<GameObject *>> Layers;

public:
  void add_new(f32 x, f32 y, u32 Layer);

  // this can get really, really expensive, so be careful
  void foreach (std::function<void(GameObject *)> func);

  void draw();
};

class IconContainer {
  DLinkedList<Icon *> Icons;

public:
  Icon *add_new(string path, i32 x, i32 y);

  void foreach (std::function<void(Icon *)> func);

  void draw();
};
