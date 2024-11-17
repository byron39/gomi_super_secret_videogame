#pragma once

#include "Colliders.hpp"
#include "list.hpp"
#include "types.hpp"
#include <raylib.h>

class GameObject {
public:
  Rectangle matrix;
  f32 rotation;
  unique_ptr<Collider *> collider;
  string texture_path;
  Texture *texture;
  u64 LayerID, UID;
};
