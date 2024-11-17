#include "../include/Selection.hpp"

Selection::Selection(shared_ptr<GameObjectContainer> GOR,
                     shared_ptr<IconContainer> IR) {
  GameObjectsRef = GOR;
  IconsRef = IR;
  Selected = new Selectable();
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
          cout << "chat, is this real\n";
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
      }
      break;
    case OBJECT:

      break;
    }
  }
}

node<Selectable *> *Selection::new_object(void *newObject,
                                          SelectableType Type) {


  // this is some crazy UB, fix this as soon as possible
  auto node = Objects.append((Selectable *)newObject);
  node->data->type = Type;
  node->data->key = node;
  node->data->ptr = newObject;
  return node;
}
