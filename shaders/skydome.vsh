#version 100

attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_norm;
attribute vec4 v_color;

varying vec3 f_pos;

void main() {
	f_pos = v_position;
	gl_Position = vec4(v_position.x, v_position.z, 0, 1);
}
