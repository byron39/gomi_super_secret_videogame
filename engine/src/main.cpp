#include "../include/Colliders.hpp"
#include "../include/types.hpp"
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

class Icon {
public:
  Texture texture;
  Vector2 pos;
  Rectangle scale;
};

bool endswith(string s, const char *end) {
  if (strlen(end) > s.size()) {
    return false;
  }
  string thisend = s.substr(s.size() - strlen(end), s.size() + 1);
  string str = end;
  return strcmp(thisend.c_str(), str.c_str());
}
int main() {

  InitWindow(0, 0, "levelbuilder");

  i32 monitor = GetCurrentMonitor();

  string path = fs::current_path();

  vector<Icon> Icons;

  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_regular_file()) {
      if (endswith(entry.path(), ".png")) {
        Icons.push_back();
      }
    }
  }

  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  if (!IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
  }

  CloseWindow();
}
