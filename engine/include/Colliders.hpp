#pragma once

#include "types.hpp"
#include <raylib.h>

typedef enum ColliderType {
  NONE = 0,
  POLYGON = 1,
  RECTANGLE = 2,
  CIRCLE = 3,
} ColliderType;

class Collider {
  public:
    ColliderType type;
};

typedef struct None : Collider {
  // don't you fucking dare question this
} None;

typedef struct Circle : Collider {
  Vector2 center;
  f32 radius;
} Circle;

typedef struct rectangle : Collider {
  i32 top, left, width, height;
} rectangle;

typedef struct Polygon : Collider {
  vector<Vector2> vertecies;
} Polygon;
