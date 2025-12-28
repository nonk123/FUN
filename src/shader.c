#include <raylib.h>

#include "log.h"
#include "shader.h"

static const char* VARS[] = {
	[SHV_TEXTURE] = "texture0",
	[SHV_AMBIENT] = "ambient",
	[SHV_LIGHT_COUNT] = "light_count",
	[SHV_UV_SCALE] = "uv_scale",
};

Shader shaders[SHT_COUNT] = {0};

static const char *base_vsh =
#include "shaders/base.vsh"
	, *base_fsh =
#include "shaders/base.fsh"
		  , *skydome_fsh =
#include "shaders/skydome.fsh"
	;

Material* make_materials() {
	Material* material = MemAlloc(sizeof(*material));
	material->maps = MemAlloc(sizeof(*material->maps));
	material->maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	material->shader = shaders[SHT_BASIC];
	return material;
}

void sh_init() {
	const char* sources[SHT_COUNT][2] = {
		[SHT_BASIC] = {base_vsh, base_fsh   },
		[SHT_SKYDOME] = {base_vsh, skydome_fsh},
	};

	for (int i = 0; i < SHT_COUNT; i++) {
		const char *vsh = sources[i][0], *fsh = sources[i][1];

		shaders[i] = LoadShaderFromMemory(vsh, fsh);
		expect(shaders[i].id, "Failed to load shader %d", i);

		shaders[i].locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shaders[i], "mvp");
		shaders[i].locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shaders[i], "m_model");

		const int zero = 0;
		sh_set(SHV_LIGHT_COUNT, &zero, SHADER_UNIFORM_INT);
	}
}

void sh_teardown() {
	for (int i = 0; i < SHT_COUNT; i++)
		UnloadShader(shaders[i]);
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
	SetShaderValueV(shaders[SHT_BASIC], GetShaderLocation(shaders[SHT_BASIC], name), value, type, count);
}
