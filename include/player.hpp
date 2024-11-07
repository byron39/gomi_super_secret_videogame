#pragma once

#include "time.hpp"
#include "types.hpp"
#include <raylib.h>

typedef enum { UP, LEFT, DOWN, RIGHT } DIRECTIONS;

class Player {
public:
  bool isMoving;
  f32 *targetFrameTime;
  i32 animation_cooldown;
  i32 animation_interval;
  DIRECTIONS dir;
  Rectangle src, world_dst;
  Texture2D texture;
  f32 speed;
  Player(Time *t);
  ~Player();
  void render();
  void update();
};
