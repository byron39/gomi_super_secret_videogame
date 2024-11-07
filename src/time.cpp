#include "../include/time.hpp"
#include <raylib.h>

Time::Time(i32 asked_for_fps) {
  targetFps = asked_for_fps;
  SetTargetFPS(targetFps);
  deltaTime = GetFrameTime();
}

void Time::update() { deltaTime = GetFrameTime(); }
