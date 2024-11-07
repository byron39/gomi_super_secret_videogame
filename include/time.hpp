#pragma once

#include "types.hpp"
#include <raylib.h>

class Time {
public:
  i32 targetFps;
  f32 deltaTime; // in millieseconds
  Time(i32 asked_for_fps);
  void update();
};
