#pragma once

#include "types.hpp"
#include <functional>

template <typename T> class node {
public:
  node<T> *last;
  T data;
  node<T> *next;
};

template <typename T> class DLinkedList {
  node<T> *head;

public:
  DLinkedList() {
    this->head->last = nullptr;
    this->head->next = nullptr;
    this->head->data = nullptr;
  };
  void erase(node<T> *n) {
    n->last->next = n->next;
    n->next->last = n->last;

    delete n;
  };
  node<T> *append(T new_data) {
    head->data = new_data;
    node<T> *new_node = new node<T>;
    new_node->last = head;
    new_node->data = nullptr;
    new_node->next = nullptr;
    head->next = new_node;

    return head->last;
  };
  void foreach (std::function<void(T)> func) {
    node<T> *tmp = head;
    while (tmp->last) {
      func(tmp->data);
      tmp = tmp->last;
    }
  };
  ~DLinkedList() {
    node<T> *tmp;
    while (head->last) {
      tmp = head;
      head = head->last;
      delete tmp;
    }
  };
};
