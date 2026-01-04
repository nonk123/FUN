#include <raylib.h>

#include "log.h"
#include "shader.h"

static const char* VARS[] = {
	[SHV_AMBIENT] = "ambient",
	[SHV_LIGHT_COUNT] = "light_count",
};

Shader shaders[SHT_COUNT] = {0}; // extern in `skydome.c`

// clang-format off
static const char *leet_vsh =
#include "shaders/leet.vsh"
	, *leet_fsh =
#include "shaders/leet.fsh"
	, *skydome_vsh =
#include "shaders/skydome.vsh"
	, *skydome_fsh =
#include "shaders/skydome.fsh"
	;
// clang-format on

Material* make_leet_materials() {
	Material* material = MemAlloc(sizeof(*material));
	material->maps = MemAlloc(8 * sizeof(*material->maps));
	material->maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	material->shader = shaders[SHT_LEET];
	return material;
}

void sh_init() {
	// clang-format off
	const char* sources[SHT_COUNT][2] = {
		[SHT_LEET] = {leet_vsh, leet_fsh},
		[SHT_SKYDOME] = {skydome_vsh, skydome_fsh},
	};
	// clang-format on

	for (int i = 0; i < SHT_COUNT; i++) {
		const char *vsh = sources[i][0], *fsh = sources[i][1];
		shaders[i] = LoadShaderFromMemory(vsh, fsh);
		expect(shaders[i].id, "Failed to load shader %d", i);

		shaders[i].locs[SHADER_LOC_VERTEX_POSITION] = GetShaderLocationAttrib(shaders[i], "v_pos");
		shaders[i].locs[SHADER_LOC_VERTEX_TEXCOORD01] = GetShaderLocationAttrib(shaders[i], "v_uv");
		shaders[i].locs[SHADER_LOC_VERTEX_NORMAL] = GetShaderLocationAttrib(shaders[i], "v_norm");
		shaders[i].locs[SHADER_LOC_VERTEX_COLOR] = GetShaderLocationAttrib(shaders[i], "v_color");
		shaders[i].locs[SHADER_LOC_VERTEX_TANGENT] = GetShaderLocationAttrib(shaders[i], "v_tangent");

		shaders[i].locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shaders[i], "mvp");
		shaders[i].locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shaders[i], "m_model");

		shaders[i].locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(shaders[i], "albedo");
		shaders[i].locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(shaders[i], "normal_map");
	}

	const int zero = 0;
	sh_set(SHT_LEET, SHV_LIGHT_COUNT, &zero, SHADER_UNIFORM_INT);
}

void sh_teardown() {
	for (int i = 0; i < SHT_COUNT; i++)
		UnloadShader(shaders[i]);
}

void sh_set(ShaderType shader, ShaderValue idx, const void* value, int type) {
	const char* var = VARS[idx];
	expect(var, "Undefined shader-var: %d", idx);
	sh_set_raw(shader, var, value, type);
}

void sh_set_raw(ShaderType shader, const char* name, const void* value, int type) {
	SetShaderValueV(shaders[shader], GetShaderLocation(shaders[shader], name), value, type, 1);
}
