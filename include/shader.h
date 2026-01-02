#pragma once

#include <raylib.h>

#define RGBA(r, g, b, a) ((float[4]){r, g, b, a})
#define RGB(r, g, b) RGBA(r, g, b, 1.f)

typedef enum {
	SHT_LEET,
	SHT_SKYDOME,
	SHT_COUNT,
} ShaderType;

extern Shader shaders[SHT_COUNT];

typedef enum {
	SHV_AMBIENT,
	SHV_LIGHT_COUNT,
} ShaderValue;

void sh_init(), sh_teardown();

void sh_set(ShaderType, ShaderValue, const void* value, int type);
void sh_set_raw(ShaderType, const char* name, const void* value, int type);

Material* make_leet_materials();
