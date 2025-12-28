#include <stdint.h>
#include <string.h>

#include "light.h"
#include "shader.h"
#include "vecmath.h"

static Light lights[128] = {0};
static int light_count = 0;

static void check_light_count() {
	if (light_count >= MAX_LIGHTS)
		light_count = MAX_LIGHTS;
}

void light_reset() {
	memset(lights, 0, sizeof(lights));
	light_count = 0;
}

static void light_set(int idx, const char* field, const void* value, int type) {
	sh_set_raw(SHT_LEET, TextFormat("lights[%d].%s", idx, field), value, type);
}

void light_done() {
	check_light_count();
	sh_set(SHT_LEET, SHV_LIGHT_COUNT, &light_count, SHADER_UNIFORM_INT);

	for (int i = 0; i < MAX_LIGHTS; i++) {
		light_set(i, "type", &lights[i].type, SHADER_UNIFORM_INT);
		light_set(i, "position", &lights[i].position, SHADER_UNIFORM_VEC3);
		light_set(i, "aux", &lights[i].aux, SHADER_UNIFORM_VEC3);
		light_set(i, "color", &lights[i].color, SHADER_UNIFORM_VEC4);
	}
}

void light_pos(float x, float y, float z) {
	light_pos_v(XYZ(x, y, z));
}

void light_pos_v(Vector3 position) {
	lights[light_count].position = position;
}

void light_dir(float x, float y, float z) {
	light_dir_v(XYZ(x, y, z));
}

void light_dir_v(Vector3 direction) {
	lights[light_count].type = LIGHT_DIRECTIONAL;
	lights[light_count].aux = Vector3Normalize(direction);
}

void light_radius(float radius) {
	lights[light_count].type = LIGHT_POINT;
	lights[light_count].aux.x = radius;
}

void light_color(float r, float g, float b, float a) {
	lights[light_count].color = (Vector4){.x = r, .y = g, .z = b, .w = a};
}

void light_color_v(Color color) {
	light_color((float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f);
}

void place_light() {
	light_count++, check_light_count();
}
