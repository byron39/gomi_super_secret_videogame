#pragma once

#include "Colliders.hpp"
#include "list.hpp"
#include "selectable.hpp"
#include "types.hpp"
#include "uids.hpp"
#include <boost/optional.hpp>
#include <boost/optional/optional.hpp>
#include <raylib.h>

class GameObject : public Selectable {
public:
  Rectangle matrix;
  Vector2 scale;
  f32 rotation;
  Collider collider;
  string texture_path;
  Texture *texture;
  u64 UID;
  u8 layer_id;
  node<GameObject *> *ObjectListKey;

  GameObject(Vector2 pos, Texture *textureRef, string &textPath, u8 Layer);
};
