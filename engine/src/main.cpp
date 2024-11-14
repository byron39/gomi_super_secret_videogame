#include "../include/Colliders.hpp"
#include "../include/Icons.hpp"
#include <cstring>
#include <filesystem>
#include <raylib.h>

class UIDGenerator {
  static u64 current;

public:
  static u64 GetNewUid() { return current++; }
};

u64 UIDGenerator::current = 0;

class GameObject {
  Vector2 pos, scale;
  f32 rotation;
  unique_ptr<Collider *> collider;
  string texture_path;
  u64 LayerID, UID;
};

class Scene {
  map<u64, GameObject *> Objects;
};

bool endswith(string s, const char *end) {
  if (strlen(end) > s.size()) {
    return false;
  }
  string thisend = s.substr(s.size() - strlen(end), s.size() + 1);
  string str = end;
  return strcmp(thisend.c_str(), str.c_str()) == 0;
}

int main() {

  InitWindow(0, 0, "levelbuilder");

  i32 monitor = GetCurrentMonitor();

  string path = fs::current_path();

  vector<Icon> Icons;

  i32 left_offset = 0;

  i32 top_offset = 0;

  Rectangle SelectionWindow = {0, 0, BOX_WIDTH * 2, f32(GetScreenWidth())};

  SetTargetFPS(60);

  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_regular_file()) {
      if (endswith(entry.path(), ".png")) {
        Icons.push_back(
            Icon(entry.path(), left_offset * BOX_WIDTH, top_offset * 128));
        left_offset++;
        cout << left_offset * BOX_WIDTH << endl;
        if (left_offset * BOX_WIDTH >= BOX_WIDTH * 2) {
          left_offset = 0;
          top_offset++;
        }
      }
    }
  }

  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  if (!IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  while (!WindowShouldClose()) {
    BeginDrawing();

    DrawRectangle(SelectionWindow.x, SelectionWindow.y, SelectionWindow.width,
                  SelectionWindow.height, Color{176, 176, 176, 255});
    for (auto icon : Icons) {
      icon.draw();
    }

    EndDrawing();
  }

  CloseWindow();
}
