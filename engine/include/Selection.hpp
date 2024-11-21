#pragma once
#include "GameObjects.hpp"
#include "Icons.hpp"
#include "containers.hpp"
#include "list.hpp"
#include "selectable.hpp"
#include "types.hpp"
#include <boost/static_assert.hpp>
#include <memory>
#include <raylib.h>

class Selection {

  DLinkedList<Selectable *> Objects; // all objects get added here

  Selectable *Selected = nullptr;
  bool isSelected;
  Camera2D *camera;

  shared_ptr<GameObjectContainer> GameObjectsRef;
  shared_ptr<IconContainer> IconsRef;

public:
  u8 currentLayer;
  Selection(shared_ptr<GameObjectContainer> GOR, shared_ptr<IconContainer> IR,
            Camera2D *cameraRef);

  void update(Rectangle *selectionWindow);

  template <typename ObjectType>
  node<Selectable *> *new_object(Selectable *newObject, SelectableType Type) {

    BOOST_STATIC_ASSERT((std::is_base_of<Selectable, ObjectType>::value));

    // this is some crazy UB, fix this as soon as possible
    auto node = Objects.append(newObject);
    node->data->type = Type;
    node->data->key = node;
    node->data->ptr = newObject;
    return node;
  }
};
