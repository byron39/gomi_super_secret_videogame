#pragma once
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
// lots of quality of life stuff here
//

using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::map;
using std::ofstream;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

using std::make_unique;

#define let const auto // we do a little trolling
#define mut auto       // im landing in hell for this

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef struct Vec2 {
  f32 x;
  f32 y;
} Vec2;

typedef struct Vec3 {
  f32 x;
  f32 y;
  f32 z;
} Vec3;
