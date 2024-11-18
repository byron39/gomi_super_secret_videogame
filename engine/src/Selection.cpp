#include "../include/Selection.hpp"
#include "GameObjects.hpp"
#include "Icons.hpp"

Selection::Selection(shared_ptr<GameObjectContainer> GOR,
                     shared_ptr<IconContainer> IR) {
  GameObjectsRef = GOR;
  IconsRef = IR;
  Selected = new Selectable();
  currentLayer = 0;
}

void Selection::update(Rectangle *selectionWindow) {

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 Mouse = GetMousePosition();
    Objects.foreach ([this, Mouse](Selectable *object) {
      switch (object->type) {
      case ICON:
        if (CheckCollisionPointRec(Mouse, ((Icon *)object->ptr)->scale)) {
          this->Selected->ptr = (Selectable *)object->ptr;
          this->Selected->type = ICON;
        }
        break;
      case OBJECT:
        GameObject *p = (GameObject *)object->ptr;
        if (CheckCollisionPointRec(Mouse, p->matrix)) {
          this->Selected->ptr = (Selectable *)object->ptr;
          this->Selected->type = OBJECT;
          this->Selected->key = object->key;
        }
        break;
      }
    });
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && Selected) {
    Vector2 Mouse = GetMousePosition();
    Rectangle WindowRec = {0, 0, (f32)GetScreenWidth(), (f32)GetScreenHeight()};
    switch (Selected->type) {
    case ICON:
      if (!CheckCollisionPointRec(Mouse, *selectionWindow) &&
          CheckCollisionPointRec(Mouse, WindowRec)) {
        auto p = (Icon *)Selected->ptr;
        GameObjectsRef->add_new(p->scale.x, p->scale.y, currentLayer);
      }
      break;
    case OBJECT:

      break;
    }
  }
}
