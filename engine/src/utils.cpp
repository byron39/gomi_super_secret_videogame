#include "../include/utils.hpp"
#include <cstring>
#include <raylib.h>

bool endswith(string s, const char *end) {
  if (strlen(end) > s.size()) {
    return false;
  }
  string thisend = s.substr(s.size() - strlen(end), s.size() + 1);
  string str = end;
  return strcmp(thisend.c_str(), str.c_str()) == 0;
}

Rectangle RecWorldToScreen(Rectangle *rec, Camera2D *camera) {
  Vector2 tmp = GetWorldToScreen2D({rec->x, rec->y}, *camera);
  return {tmp.x, tmp.y, rec->width, rec->height};
}
Rectangle RecScreenToWorld(Rectangle *rec, Camera2D *camera) {
  Vector2 tmp = GetScreenToWorld2D({rec->x, rec->y}, *camera);
  return {tmp.x, tmp.y, rec->width, rec->height};
}
