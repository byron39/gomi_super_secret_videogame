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
  f32 rotation;
  boost::optional<Collider> collider;
  string texture_path;
  Texture *texture;
  u64 LayerID, UID;

  GameObject(f32 x, f32 y, Texture *textureRef, string &path);
};
