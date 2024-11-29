#include "../include/GameObjects.hpp"

GameObject::GameObject(Vector2 pos, Texture *textureRef, string &textPath, u8 Layer) {
  this->texture = textureRef;
  this->matrix.x = pos.x;
  this->matrix.y = pos.y;
  this->matrix.width = textureRef->width;
  this->matrix.height = textureRef->height;

  this->rotation = 0;
  this->scale = {0, 0};
  this->collider = Collider();
  this->texture_path = textPath;
  this->UID = UIDGenerator::GetNewUid();
  this->layer_id = Layer;
  this->type = OBJECT;
}
