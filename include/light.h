#pragma once

#include <raylib.h>

#define MAX_LIGHTS (128)

typedef struct {
	Vector3 position;
	Vector4 color;
} Light;

void light_reset(), light_done();

void light_pos(float, float, float), light_pos_v(Vector3);
void light_color(float, float, float, float), light_color_v(Color);
void place_light();
