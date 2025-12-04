#include <stdio.h>

#include <raylib.h>

#include "light.h"
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

static const char* fsh_fmt = R"(
#version 100

#define MAX_LIGHTS (%d)
#define LIGHT_DIRECTIONAL (%d)
#define LIGHT_POINT (%d)

precision mediump float;

varying vec3 f_pos;
varying vec2 f_uv;
varying vec3 f_norm;
varying vec4 f_color;

uniform sampler2D texture0;
uniform vec4 ambient;

uniform int light_count;
uniform struct {
	int type;
	vec3 position, aux;
	vec4 color;
} lights[MAX_LIGHTS];

void main() {
	vec4 texel_color = texture2D(texture0, f_uv);
	vec4 surface_color = f_color * texel_color;

	vec4 light_color = ambient;
	for (int i = 0; i < light_count; i++) {
		float diffuse = 0.0;
		if (lights[i].type == LIGHT_POINT) {
			vec3 light_dist = lights[i].position - f_pos;
			float fact = clamp(lights[i].aux.x / length(light_dist), 0.0, 1.0);
			vec3 light_dir = normalize(light_dist);
			diffuse = max(0.0, fact * dot(f_norm, light_dir));
		}
		light_color += lights[i].color * diffuse;
	}

	light_color = min(light_color, vec4(1.0));
	vec4 final_color = light_color * surface_color;

	vec3 shadowed = mix(vec3(0.0), final_color.rgb, final_color.a);
	final_color = vec4(shadowed, surface_color.a);

	/* gl_FragColor = vec4(f_norm, 1.0); */
	gl_FragColor = min(final_color, vec4(1.0));
}
)";

static Shader shader = {0};

Shader sh_get() {
	return shader;
}

void sh_init() {
	char fsh[4096] = "";
	snprintf(fsh, sizeof(fsh), fsh_fmt, MAX_LIGHTS, LIGHT_DIRECTIONAL, LIGHT_POINT);

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
