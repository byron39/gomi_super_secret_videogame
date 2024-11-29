#include "../include/player.hpp"
#include <raylib.h>

Player::Player(Time *t) {
  targetFrameTime = &t->deltaTime;
  this->texture = LoadTexture("assets/player_1.png");

  this->speed = 100;
  this->src = {0, 0, 64, 64};
  this->world_dst = {0, 0, 128, 128};

  animation_cooldown = 0;
  animation_interval = t->targetFps / 7;
  isMoving = false;
}
Player::~Player() { UnloadTexture(texture); }

void Player::render() {
  DrawTexturePro(this->texture, this->src, this->world_dst, {32, 32}, 0.0,
                 WHITE);
}
void Player::update() {

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      src.x = 0;

    switch (dir) {
    case UP:
      src.y = 1024;
    case LEFT:
      src.y = 1088;
    case DOWN:
      src.y = 1152;
    case RIGHT:
      src.y = 1216;
    default:
      break;
    }

    if (animation_cooldown == animation_interval) {
      animation_cooldown = 0;
      src.x += 64;
      if (src.x == 640) {
        src.x = 0;
        // TODO: add arrow mechanic
      }
    }
    animation_cooldown++;

  } else {
    if (IsKeyDown(KEY_W)) {
      world_dst.y -= speed * (*targetFrameTime);
      dir = UP;
      isMoving = true;
    } else if (IsKeyDown(KEY_A)) {
      world_dst.x -= speed * (*targetFrameTime);
      dir = LEFT;
      isMoving = true;
    } else if (IsKeyDown(KEY_S)) {
      world_dst.y += speed * (*targetFrameTime);
      dir = DOWN;
      isMoving = true;
    } else if (IsKeyDown(KEY_D)) {
      world_dst.x += speed * (*targetFrameTime);
      dir = RIGHT;
      isMoving = true;
    } else {
      isMoving = false;
    }
    if (isMoving) {
      switch (dir) {
      case UP:
        src.y = 512;
        break;
      case LEFT:
        src.y = 576;
        break;
      case DOWN:
        src.y = 640;
        break;
      case RIGHT:
        src.y = 706;
        break;
      default:
        break;
      }

      if (animation_cooldown == animation_interval) {
        animation_cooldown = 0;
        src.x += 64;
        if (src.x == 576) {
          src.x = 0;
        }
      }
      animation_cooldown++;
    }
  }
}
