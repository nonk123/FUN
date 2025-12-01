#include <raylib.h>

#include "log.h"
#include "shader.h"

static const char* VARS[] = {
	[SHV_TEXTURE] = "texture0",
	[SHV_AMBIENT] = "ambient",
};

static const char* vsh = R"(
#version 100

attribute vec3 v_position;
attribute vec2 v_tex_coord;
attribute vec3 v_normal;
attribute vec4 v_color;

uniform mat4 mvp;

varying vec2 f_tex_coord;
varying vec4 f_color;

void main() {
	f_tex_coord = v_tex_coord;
	f_color = v_color;
	gl_Position = mvp * vec4(v_position, 1.0);
}
)";

static const char* fsh = R"(
#version 100

precision mediump float;

varying vec2 f_tex_coord;
varying vec4 f_color;

uniform sampler2D texture0;
uniform vec4 ambient;

void main() {
	vec4 texel_color = texture2D(texture0, f_tex_coord);
	vec4 surface_color = f_color * texel_color;
	gl_FragColor = vec4(mix(surface_color.rgb, ambient.rgb, ambient.a), surface_color.a);
}
)";

static Shader shader = {0};

void sh_init() {
	shader = LoadShaderFromMemory(vsh, fsh);
	expect(shader.id, "Failed to load shader");
	// shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
	shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
}

void sh_set(ShaderValue idx, const void* value, int type) {
	sh_set_v(idx, value, type, 1);
}

void sh_set_v(ShaderValue idx, const void* value, int type, int count) {
	idx = GetShaderLocation(shader, VARS[idx]);
	SetShaderValueV(shader, idx, value, type, count);
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
