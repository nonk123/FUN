#pragma once

#include <raylib.h>

void t_init(), t_teardown();
void t_restart(), t_update(), t_draw();

float t_height(float x, float z);
Vector3 t_norm(float x, float z);
