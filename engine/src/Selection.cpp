#include "../include/Selection.hpp"
#include <raylib.h>

void Selection::update(Rectangle *selectionWindow) {

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 Mouse = GetMousePosition();
    Objects.foreach ([this, Mouse](Selectable *object) {
      switch (object->type) {
      case ICON:
        if (CheckCollisionPointRec(Mouse, ((Icon *)object->ptr)->scale)) {
          this->Selected->ptr = (Selectable *)object->ptr;
          this->Selected->type = ICON;
          this->Selected->key = object->key;
        }
        break;
      case OBJECT:
        GameObject *p = (GameObject *)object->ptr;
        Rectangle tmp = {p->pos.x, p->pos.y, p->scale.x, p->scale.y};
        if (CheckCollisionPointRec(Mouse, tmp)) {
          this->Selected->ptr = (Selectable *)object->ptr;
          this->Selected->type = OBJECT;
          this->Selected->key = object->key;
        }
        break;
      }
    });
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    Vector2 Mouse = GetMousePosition();
    switch (Selected->type) {
    case ICON:
      if (!CheckCollisionPointRec(Mouse, *selectionWindow) &&
          CheckCollisionPointRec(Mouse, )) {
      }
      break;
    case OBJECT:

      break;
    }
  }
}
