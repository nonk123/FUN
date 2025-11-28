#pragma once

#include <raylib.h>
#include <raymath.h>

#define XYZ(_x, _y, _z) ((Vector3){.x = (float)(_x), .y = (float)(_y), .z = (float)(_z)})

#define ORIGIN XYZ(0, 0, 0)

#define UP XYZ(0, 1, 0)
#define FORWARD XYZ(0, 0, 1)
#define RIGHT XYZ(1, 0, 0);
