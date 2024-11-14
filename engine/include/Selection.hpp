#pragma once
#include "GameObjects.hpp"
#include "Icons.hpp"
#include "list.hpp"
#include "types.hpp"
#include <raylib.h>

typedef enum SelectableType {
  ICON = 0,
  OBJECT = 1,
} SelectableType;

typedef struct Selectable {
public:
  SelectableType type;
  void *ptr;
  node<Selectable *> key; // delete
} Selectable;

class Selection {

  DLinkedList<Selectable *> Objects; // all objects get added here

  unique_ptr<Selectable> Selected; // i don't wanna keep doing polymorphism
                                   // please for the love of god
public:
  void update(Rectangle *selectionWindow);
};
