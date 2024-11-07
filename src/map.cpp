#include "../include/map.hpp"
#include <raylib.h>

Map::Map() {
  texture = LoadTexture("assets/new_map.png");
  dst = {0, 0, 2048 * 7, 2048 * 7};
  src = {0, 0, 2048, 2048};
}

Map::~Map() { UnloadTexture(texture); }

void Map::render() { DrawTexturePro(texture, src, dst, {0}, 0, WHITE); }
