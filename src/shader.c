#include <raylib.h>

#include "shader.h"

static const char* VARS[] = {
	[SHV_TEXTURE] = "texture0",
	[SHV_DIFFUSE] = "diffuse",
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
uniform vec4 diffuse;

void main() {
	vec4 texel_color = texture2D(texture0, f_tex_coord);
	gl_FragColor = texel_color * diffuse;
}
)";

static Shader shader = {0};

void sh_init() {
	shader = LoadShaderFromMemory(vsh, fsh);
	// shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
	shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
	sh_set(SHV_DIFFUSE, RGB(1.f, 0.f, 0.f), SHADER_UNIFORM_VEC4);
}

void sh_set(ShaderValue idx, const void* value, int type) {
	idx = GetShaderLocation(shader, VARS[idx]);
	SetShaderValue(shader, idx, value, type);
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
