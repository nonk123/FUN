#include <raylib.h>

#include "log.h"
#include "shader.h"

static const char* VARS[] = {
	[SHV_TEXTURE] = "texture0",
	[SHV_AMBIENT] = "ambient",
	[SHV_LIGHT_COUNT] = "light_count",
};

static const char* vsh = R"(
#version 100

attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_norm;
attribute vec4 v_color;

uniform mat4 mvp;
uniform mat4 m_model;

varying vec3 f_pos;
varying vec2 f_uv;
varying vec3 f_norm;
varying vec4 f_color;

void main() {
	f_pos = vec3(m_model * vec4(v_position, 1.0));
	f_uv = v_uv, f_norm = v_norm, f_color = v_color;
	gl_Position = mvp * vec4(v_position, 1.0);
}
)";

static const char* fsh = R"(
#version 100

#define MAX_LIGHTS (128)

precision mediump float;

varying vec3 f_pos;
varying vec2 f_uv;
varying vec3 f_norm;
varying vec4 f_color;

uniform sampler2D texture0;
uniform vec4 ambient;

uniform int light_count;
uniform struct {
	vec3 pos;
	vec4 color;
} lights[MAX_LIGHTS];

void main() {
	vec4 texel_color = texture2D(texture0, f_uv);
	vec4 surface_color = f_color * texel_color;

	vec4 light_color = ambient;
	for (int i = 0; i < light_count; i++) {
		vec3 light_dir = normalize(lights[i].pos - f_pos);
		float diffuse = max(dot(f_norm, light_dir), 0.0);
		light_color += diffuse * lights[i].color;
	}
	light_color = min(light_color, vec4(1.0));

	gl_FragColor = light_color * surface_color;
}
)";

static Shader shader = {0};

void sh_init() {
	shader = LoadShaderFromMemory(vsh, fsh);
	expect(shader.id, "Failed to load shader");

	shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
	shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "m_model");

	const int zero = 0;
	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);
	sh_set(SHV_LIGHT_COUNT, &zero, SHADER_UNIFORM_INT);
}

void sh_set(ShaderValue idx, const void* value, int type) {
	sh_set_v(idx, value, type, 1);
}

void sh_set_v(ShaderValue idx, const void* value, int type, int count) {
	sh_set_raw(VARS[idx], value, type, count);
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
