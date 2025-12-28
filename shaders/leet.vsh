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
