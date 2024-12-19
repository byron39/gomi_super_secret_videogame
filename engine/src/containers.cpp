#include "../include/containers.hpp"
#include <raylib.h>

// don't show this to anyone i beg of you
void IconContainer::foreach (std::function<void(TextureIcon *)> func) {
  Icons.foreach (func);
}

void IconContainer::draw(Rectangle *barFrame) {

  Icons.foreach ([barFrame](TextureIcon *icon) { icon->draw(barFrame); });
}
TextureIcon *IconContainer::add_new(string path, i32 x, i32 y) {

  TextureIcon *newIcon = new TextureIcon(path, x, y);
  auto ref = Icons.append(newIcon);
  newIcon->make_ref(ref);
  return newIcon;
}

GameObject *GameObjectContainer::add_new(TextureIcon *icon, Camera2D *camera,
                                         u8 Layer) {
  if (Layer >= Layers.size()) {
    for (i32 i = Layers.size(); i <= Layer; i++) {
      Layers.push_back(new DLinkedList<GameObject *>());
    }
  }

  Vector2 mouse = GetMousePosition();
  auto newObject = new GameObject(GetScreenToWorld2D(mouse, *camera),
                                  &icon->texture, icon->text, Layer);
  auto ref = Layers[Layer]->append(newObject);
  newObject->ObjectListKey = ref;
  return newObject;
}

void GameObjectContainer::foreach (std::function<void(GameObject *)> func) {
  for (auto layer : Layers) {
    layer->foreach (func);  // Use std::for_each ?
  }
}

void GameObjectContainer::draw(Camera2D *camera) {

  for (auto layer : Layers) {
    layer->foreach ([camera](GameObject *obj) {
      Vector2 screen_pos =
          GetWorldToScreen2D({obj->matrix.x, obj->matrix.y}, *camera);
      Rectangle drawMatrix = {screen_pos.x, screen_pos.y,
                              obj->matrix.width * camera->zoom,
                              obj->matrix.height * camera->zoom};
      DrawTexturePro(
          *obj->texture,
          {0, 0, (f32)obj->texture->width, (f32)obj->texture->height},
          drawMatrix, {0, 0}, obj->rotation, RAYWHITE);
    });
  }
}
