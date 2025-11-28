#pragma once

#include <stdbool.h>

#include <raylib.h>

#include "vecmath.h"

void set_lock_cursor(bool);
void look_at(Vector3 from, Vector3 to);
void look_dir(Vector3 from, Vector3 dir);
