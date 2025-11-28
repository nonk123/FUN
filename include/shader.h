#pragma once

#define RGBA(r, g, b, a) ((float[4]){r, g, b, a})
#define RGB(r, g, b) RGBA(r, g, b, 1.f)

typedef enum {
	SHV_TEXTURE,
	SHV_DIFFUSE,
} ShaderValue;

void shader_init(), shader_teardown();
void shader_begin(), shader_end();
void shader_set(ShaderValue idx, const void* value, int type);
