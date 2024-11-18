#pragma once
#include "list.hpp"

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
