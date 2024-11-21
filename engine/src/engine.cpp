#include "../include/engine.hpp"
#include <raylib.h>

EngineState::EngineState() {

  // uggggghhhhhhhhhhhhhhhhhhhhhhhhh

  Objects = make_shared<GameObjectContainer>();
  icons = make_shared<IconContainer>();

  selection = make_unique<Selection>(Objects, icons, &SceneCam);

  InitWindow(0, 0, "levelbuilder");

  i32 monitor = GetCurrentMonitor();

  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  if (!IsWindowFullscreen()) {
    ToggleFullscreen();
  }
  string path = fs::current_path();
  if (path.find("build") != string::npos) {
    path = path.substr(0, path.find("build"));
  }

  cout << path << endl;

  i32 left_offset = 0;

  i32 top_offset = 0;

  SceneCam.target = {(f32)GetScreenWidth() / 2, (f32)GetScreenHeight() / 2};
  SceneCam.rotation = 0;
  SceneCam.offset = {(f32)GetScreenWidth() / 2, (f32)GetScreenHeight() / 2};
  SceneCam.zoom = 1;

  selectionWindow = {0, 0, BOX_WIDTH * 2, f32(GetScreenWidth())};

  SetTargetFPS(60);

  // sweet mother of jesus
  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_regular_file()) {
      if (endswith(entry.path(), ".png")) {
        auto icon = icons->add_new(entry.path(), left_offset * BOX_WIDTH,
                                   top_offset * BOX_WIDTH);
        selection->new_object<Icon>(icon, ICON);
        left_offset++;
        cout << left_offset * BOX_WIDTH << endl;
        if (left_offset * BOX_WIDTH >= BOX_WIDTH * 2) {
          left_offset = 0;
          top_offset++;
        }
      }
    }
  }

  this->loop();
}

EngineState::~EngineState() {
  this->selection.reset();
  this->Objects->foreach ([](GameObject *obj) { delete obj; });
  this->Objects.reset();
  this->icons->foreach ([](Icon *icon) { delete icon; });
  this->icons.reset();
}

void EngineState::loop() {

  while (!WindowShouldClose()) {

    selection->update(&selectionWindow);

    if (SceneCam.zoom + GetMouseWheelMove() * 0.02 > 0.0899998) {
      SceneCam.zoom += (f32)GetMouseWheelMove() * 0.02;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    DrawRectangleRec(selectionWindow, Color{140, 140, 140, 255});
    icons->draw();
    Objects->draw(&SceneCam);
    EndDrawing();
  }
}
