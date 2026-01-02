#version 100

attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_norm;
attribute vec4 v_color;
attribute vec3 v_tangent;

uniform mat4 mvp;
uniform mat4 m_model;
uniform mat4 m_normal;

varying vec3 f_pos;
varying vec2 f_uv;
varying vec4 f_color;
varying mat3 f_tbn;

void main() {
	// Thanks: <https://learnopengl.com/Advanced-Lighting/Normal-Mapping>.
	vec3 T = normalize(vec3(m_normal * vec4(v_tangent, 0.0)));
	vec3 N = normalize(vec3(m_normal * vec4(v_norm, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	f_tbn = mat3(T, B, N), f_pos = vec3(m_model * vec4(v_position, 1.0));
	f_uv = v_uv, f_color = v_color;
	gl_Position = mvp * vec4(v_position, 1.0);
}
