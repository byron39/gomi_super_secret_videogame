#include "../include/Colliders.hpp"
#include "../include/Icons.hpp"
#include "../include/engine.hpp"
#include <cstring>
#include <filesystem>
#include <raylib.h>

class UIDGenerator {
  static u64 current;

public:
  static u64 GetNewUid() { return current++; }
};

u64 UIDGenerator::current = 0;

class Scene {
  map<u64, GameObject *> Objects;
};
int main() {
  EngineState state;
  return 0;
}
