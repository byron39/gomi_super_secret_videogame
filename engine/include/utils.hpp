#pragma once

#include "types.hpp"
#include <raylib.h>

#define debuglog(x) cout << x << endl;

bool endswith(string s, const char *end);

Rectangle RecWorldToScreen(Rectangle *rec, Camera2D *camera);
Rectangle RecScreenToWorld(Rectangle *rec, Camera2D *camera);
