#pragma once
#include "types.hpp"

class UIDGenerator {
  static u64 current;

public:
  static u64 GetNewUid() { return current++; }
};
