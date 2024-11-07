#include "../include/list.hpp"

template <typename T> DLinkedList<T>::DLinkedList() {
  this->head.last = nullptr;
  this->head.next = nullptr;
  this->head.data = nullptr;
}

template <typename T> node<T> *DLinkedList<T>::append(void *new_data) {

  head.data = new_data;
  node<T> *new_node = new node<T>;
  new_node->last = head;
  new_node->data = nullptr;
  new_node->next = nullptr;
  head.next = new_node;

  return head.last;
}

template <typename T> void DLinkedList<T>::erase(node<T> *n) {
  n->last->next = n->next;
  n->next->last = n->last;

  delete n;
}

template <typename T> DLinkedList<T>::~DLinkedList() {
  node<T> *tmp;
  while (head->last) {
    tmp = head;
    head = head.last;
    delete tmp;
  }
}

template <typename T> void DLinkedList<T>::foreach (void(func)(T)) {
  node<T> *tmp = head;
  while (tmp->last) {
    func(tmp->data);
    tmp = tmp->last;
  }
}
