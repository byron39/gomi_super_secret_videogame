#include "../include/init.hpp"
#include "../include/map.hpp"
#include "../include/player.hpp"
#include "../include/time.hpp"
#include "../include/types.hpp"
#include "raylib.h"
#include <memory>

i32 screenWidth = 2000;
i32 screenHeight = 1200;

class Gamestate {
  Camera2D camera;
  unique_ptr<Player> player;
  unique_ptr<Map> map;
  unique_ptr<Time> time;
  vector<i32> data;

  void loop() {
    while (!WindowShouldClose()) {
      player->update();
      this->render();

      camera.target = {player->world_dst.x, player->world_dst.y};
      time->update();
    }
  }

  void render() {
    BeginDrawing();

    BeginMode2D(this->camera);
    ClearBackground(BLACK);
    map->render();
    player->render();
    EndMode2D();

    EndDrawing();
  }

public:
  ~Gamestate() { CloseWindow(); }
  Gamestate() {
    data = init();
    InitWindow(screenWidth, screenHeight, "");

    int mon_id = GetCurrentMonitor();
    screenWidth  = GetMonitorWidth(mon_id) / 2;
    screenHeight = GetMonitorHeight(mon_id) / 2;
    SetWindowSize(screenWidth, screenHeight);

    time = std::make_unique<Time>(data[0]);
    this->player = std::make_unique<Player>(time.get());
    this->map = std::make_unique<Map>();
    camera.zoom = 1;
    camera.rotation = 0;
    camera.offset = {screenWidth / 2.0, screenHeight / 2.0};

    this->loop();
  }
};

int main(void) {
  Gamestate g;

  return 0;
}
