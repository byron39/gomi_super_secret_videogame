#pragma once

#include "types.hpp"

template <typename T> class node {
public:
  node<T> *last;
  T data;
  node<T> *next;
};

template <typename T> class DLinkedList {
  node<T> head;

public:
  DLinkedList<T>();
  void erase(node<T> *n);
  node<T> *append(void *new_data);
  void foreach (void(func)(T));
  ~DLinkedList<T>();
};
