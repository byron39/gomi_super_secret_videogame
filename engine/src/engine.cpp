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

  i32 left_offset = 0;

  i32 top_offset = 0;

  SceneCam.target = {(f32)GetScreenWidth() / 2, (f32)GetScreenHeight() / 2};
  SceneCam.offset = {(f32)GetScreenWidth() / 2, (f32)GetScreenHeight() / 2};
  SceneCam.rotation = 0;
  SceneCam.zoom = 1;

  UICam.zoom = 1;     // this should never change, i hope
  UICam.rotation = 0; // same goes for here
  UICam.offset = {(f32)GetScreenWidth() / 2,
                  (f32)GetScreenHeight() / 2 - Bar.barFrame.height};
  UICam.target = {(f32)GetScreenWidth() / 2,
                  (f32)GetScreenHeight() / 2 + Bar.barFrame.height / 2};

  selectionWindow = {0, 0, BOX_WIDTH * 2, f32(GetScreenWidth())};

  SetTargetFPS(60);

  // sweet mother of jesus
  TextureIcon* last = NULL;
  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_regular_file()) {
      if (endswith(entry.path(), ".png")) {
        auto icon = icons->add_new(entry.path(), left_offset * BOX_WIDTH,
                                   top_offset * BOX_WIDTH);
        last = icon;
        selection->new_object<TextureIcon>(icon, ICON);
        left_offset++;
        if (left_offset * BOX_WIDTH >= BOX_WIDTH * 2) {
          left_offset = 0;
          top_offset++;
        }
      }
    }
  }

  if (last) {
    Objects->add_new(last, &UICam, 1);
    Objects->add_new(last, &UICam, 2);
  }

  this->loop();
}

EngineState::~EngineState() {
  GameObjectContainer g_load;
  IconContainer       i_load;
#if 0
  std::string tomlfile = "engine_out_state.toml";
  std::string tomlreout = "engine_in_state.toml";

  Serializer::ToToml(tomlfile, *this->Objects.get(), *this->icons.get());

#if 0
  bool in = Serializer::FromToml(tomlfile, g_load, i_load);
  std::cout << "Loading Result: " << in << std::endl;
#endif
  Serializer::ToToml(tomlreout, g_load, i_load);
#endif
  GameObjectContainer const * g[2] = {&*this->Objects.get(), &g_load};
  IconContainer const * i[2] = {&*this->icons.get(), &i_load};
  bool ok = Serializer::VerifyLoad(g, i);
  std::cout << "Verify Load Status: " << ok << std::endl;

  this->selection.reset();
  this->Objects->foreach ([](GameObject *obj) {
    delete obj;
    obj = nullptr;
  });
  this->Objects.reset();
  this->icons->foreach ([](TextureIcon *icon) {
    delete icon;
    icon = nullptr;
  });
  this->icons.reset();
}

void EngineState::loop() {

  while (!WindowShouldClose()) {

    selection->update(&selectionWindow, Bar.icons->IsOpen);
    if (SceneCam.zoom + GetMouseWheelMove() * 0.02 > 0.0899998) {
      SceneCam.zoom += (f32)GetMouseWheelMove() * 0.02;
    }

    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
      SceneCam.target.x -= GetMouseDelta().x;
      SceneCam.target.y -= GetMouseDelta().y;
    }

    //if (IsKeyPressed(KEY_ESCAPE))
    //{
    //  glfwSetWindowShouldClose(&selectionWindow, GL_TRUE);
    //}

    Bar.update();

    BeginDrawing();
    ClearBackground(BLACK);

    Objects->draw(&SceneCam);
    DrawRectangleRec(selectionWindow, Color{140, 140, 140, 255});
    if (Bar.icons->IsOpen) {
      icons->draw(&Bar.barFrame);
    }
    Bar.draw();
    EndDrawing();
  }
}
