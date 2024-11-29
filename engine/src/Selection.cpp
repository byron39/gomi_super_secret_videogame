#include "../include/Selection.hpp"
#include <raylib.h>

Selection::Selection(shared_ptr<GameObjectContainer> GOR,
                     shared_ptr<IconContainer> IR, Camera2D *cameraRef) {
  GameObjectsRef = GOR;
  IconsRef = IR;
  Selected = new Selectable;
  currentLayer = 0;
  camera = cameraRef;
  isSelected = false;
}

void Selection::update(Rectangle *selectionWindow, bool IconsSeletable) {

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isSelected) {
    Vector2 Mouse = GetMousePosition();
    Objects.foreach ([this, Mouse, IconsSeletable](Selectable *object) {
      switch (object->type) {
      case ICON:
        if (IconsSeletable) {
          if (CheckCollisionPointRec(Mouse, ((TextureIcon *)object->ptr)->scale)) {
            this->Selected->ptr = (Selectable *)object->ptr;
            this->Selected->type = ICON;
            this->isSelected = true;
          }
        }
        break;
      case OBJECT: {
        GameObject *p = (GameObject *)object->ptr;
        if (CheckCollisionPointRec(Mouse, p->matrix)) {
          this->Selected->ptr = (Selectable *)object->ptr;
          this->Selected->type = OBJECT;
          this->Selected->key = object->key;
          this->isSelected = true;
        }
        break;
      }
      default:
        break;
      }
    });
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && Selected && isSelected) {
    Vector2 Mouse = GetMousePosition();
    Rectangle WindowRec = {0, 0, (f32)GetScreenWidth(), (f32)GetScreenHeight()};
    switch (Selected->type) {
    case ICON:
      if (!CheckCollisionPointRec(Mouse, *selectionWindow) &&
          CheckCollisionPointRec(Mouse, WindowRec)) {
        TextureIcon *p = (TextureIcon *)Selected->ptr;
        GameObjectsRef->add_new(p, camera, currentLayer);
        isSelected = false;
        Selected->ptr = nullptr;
        Selected->key = nullptr;
      }
      break;
    case OBJECT:
      break;
    default:
      break;
    }
  }
}

Selection::~Selection() { delete Selected; }
