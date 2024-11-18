#include "../include/GameObjects.hpp"

GameObject::GameObject(f32 x, f32 y, Texture *textureRef, string &path) {
  this->texture = textureRef;
  this->matrix.x = x;
  this->matrix.x = x;
  this->matrix.width = textureRef->width;
  this->matrix.height = textureRef->height;

  this->rotation = 0;
  this->collider.reset();
  this->texture_path = path;
  this->UID = UIDGenerator::GetNewUid();
  this->type = OBJECT;
}
