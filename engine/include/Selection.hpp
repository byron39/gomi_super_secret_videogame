#pragma once
#include "GameObjects.hpp"
#include "Icons.hpp"
#include "containers.hpp"
#include "list.hpp"
#include "types.hpp"
#include <memory>
#include <raylib.h>

typedef enum SelectableType {
  ICON = 0,
  OBJECT = 1,
} SelectableType;

typedef struct Selectable {
public:
  SelectableType type;
  void *ptr;
  node<Selectable *> *key; // delete
} Selectable;

class Selection {

  DLinkedList<Selectable *> Objects; // all objects get added here

  Selectable *Selected = nullptr;

  shared_ptr<GameObjectContainer> GameObjectsRef;
  shared_ptr<IconContainer> IconsRef;

public:
  Selection(shared_ptr<GameObjectContainer> GOR, shared_ptr<IconContainer> IR);

  void update(Rectangle *selectionWindow);

  node<Selectable *> *new_object(void *newObject, SelectableType Type);
};
