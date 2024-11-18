#include "../include/containers.hpp"

// don't show this to anyone i beg of you
void IconContainer::foreach (std::function<void(Icon *)> func) {
  Icons.foreach (func);
}

void IconContainer::draw() {

  Icons.foreach ([](Icon *icon) { icon->draw(); });
}
Icon *IconContainer::add_new(string path, i32 x, i32 y) {

  Icon *newIcon = new Icon(path, x, y);
  auto ref = Icons.append(newIcon);
  newIcon->make_ref(ref);
  return newIcon;
}

GameObject *GameObjectContainer::add_new(f32 x, f32 y, u8 Layer) {
  // TODO: Do this shit please i beg of you, i know its painful but you can't
  // procrastinate on it

  return nullptr;
}

void GameObjectContainer::foreach (std::function<void(GameObject *)> func) {
  for (auto layer : Layers) {
    layer.foreach (func);
  }
}

void GameObjectContainer::draw() {
  for (auto layer : Layers) {
    layer.foreach ([](GameObject *obj) {
      DrawTexturePro(
          *obj->texture,
          {0, 0, (f32)obj->texture->width, (f32)obj->texture->height},
          obj->matrix, {0, 0}, obj->rotation, RAYWHITE);
    });
  }
}
