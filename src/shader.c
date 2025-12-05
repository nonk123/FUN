#include <raylib.h>

#include "log.h"
#include "shader.h"

static const char* VARS[] = {
	[SHV_TEXTURE] = "texture0",
	[SHV_AMBIENT] = "ambient",
	[SHV_LIGHT_COUNT] = "light_count",
	[SHV_UV_SCALE] = "uv_scale",
};

static Shader shader = {0};

static const char *vsh =
#include "shaders/base.vsh"
	, *fsh =
#include "shaders/base.fsh"
	;

Material make_material() {
	Material material = LoadMaterialDefault();
	material.shader = shader;
	return material;
}

void sh_init() {
	shader = LoadShaderFromMemory(vsh, fsh);
	expect(shader.id, "Failed to load shader");

	shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
	shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "m_model");

	const int zero = 0;
	sh_set(SHV_LIGHT_COUNT, &zero, SHADER_UNIFORM_INT);
}

void sh_set(ShaderValue idx, const void* value, int type) {
	sh_set_v(idx, value, type, 1);
}

void sh_set_v(ShaderValue idx, const void* value, int type, int count) {
	const char* var = VARS[idx];
	expect(var, "Unspecified shader-var: %d", idx);
	sh_set_raw(var, value, type, count);
}

void sh_set_raw(const char* name, const void* value, int type, int count) {
	SetShaderValueV(shader, GetShaderLocation(shader, name), value, type, count);
}

void sh_teardown() {
	UnloadShader(shader);
}

void sh_begin() {
	BeginShaderMode(shader);
}

void sh_end() {
	EndShaderMode();
}
