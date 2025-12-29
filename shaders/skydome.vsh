#version 100

attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_norm;
attribute vec4 v_color;

uniform mat4 mvp;
uniform mat4 m_model;

varying vec3 f_pos;

void main() {
	f_pos = v_position;
	gl_Position = mvp * vec4(v_position, 1.0);
}
