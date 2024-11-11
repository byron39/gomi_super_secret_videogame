#include "../include/arrows.hpp"
#include "../include/list.hpp"
#include <memory>
#include <raylib.h>

void Arrow::render(Texture *text) {}

Arrow::Arrow() { cout << "help"; }
ArrowManager::ArrowManager() {
  std::make_unique<DLinkedList<Arrow *>>();
  base_texture = LoadTexture("assets/arrow.png");
}

void ArrowManager::render() {
  ArrowManager *tmp = this;

  arrows->foreach ([&tmp](Arrow *arrow) { arrow->render(&tmp->base_texture); });
}

void ArrowManager::add_new(Rectangle player_pos) { arrows->append(new Arrow); }
