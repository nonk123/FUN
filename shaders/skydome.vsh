#version 330

in vec3 v_position;
in vec2 v_uv;
in vec3 v_norm;
in vec4 v_color;

uniform mat4 mvp;

out vec3 f_pos;

void main() {
	f_pos = v_position;
	gl_Position = mvp * vec4(v_position, 1.0);
}
