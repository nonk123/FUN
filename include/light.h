#pragma once

#include <raylib.h>

#define MAX_LIGHTS (128)

typedef enum {
	LIGHT_NONE,
	LIGHT_DIRECTIONAL,
	LIGHT_POINT,
} LightType;

typedef struct {
	Vector3 pos, aux;
	Vector4 color;
	LightType type;
} Light;

void light_begin(), light_end();

void light_pos(float, float, float), light_pos_v(Vector3);
void light_color(float, float, float, float), light_color_v(Color);

// These set the `LightType`:

void light_dir(float, float, float), light_dir_v(Vector3);
void light_radius(float);

void place_light();
