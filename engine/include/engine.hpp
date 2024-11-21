#pragma once
#include "GameObjects.hpp"
#include "Icons.hpp"
#include "Selection.hpp"
#include "containers.hpp"
#include "list.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <functional>
#include <raylib.h>

class EngineState {
  shared_ptr<GameObjectContainer> Objects;
  shared_ptr<IconContainer> icons;
  unique_ptr<Selection> selection;
  Rectangle selectionWindow;
  Camera2D SceneCam;

public:
  EngineState();
  ~EngineState();
  void loop();
};
