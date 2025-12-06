#pragma once

#include <raylib.h>

#define RGBA(r, g, b, a) ((float[4]){r, g, b, a})
#define RGB(r, g, b) RGBA(r, g, b, 1.f)

typedef enum {
	SHV_TEXTURE,
	SHV_AMBIENT,
	SHV_LIGHT_COUNT,
	SHV_UV_SCALE,
} ShaderValue;

void sh_init(), sh_teardown();
void sh_begin(), sh_end();

void sh_set(ShaderValue idx, const void* value, int type);
void sh_set_v(ShaderValue idx, const void* value, int type, int count);
void sh_set_raw(const char* name, const void* value, int type, int count);

Material* make_material();
