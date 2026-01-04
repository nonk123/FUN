#version 330

in vec3 v_pos;
in vec2 v_uv;
in vec3 v_norm;
in vec4 v_color;
in vec4 v_tangent;

uniform mat4 mvp;
uniform mat4 m_model;

out vec3 f_pos;
out vec2 f_uv;
out vec3 f_norm;
out vec4 f_color;
out mat3 f_tbn;

void main() {
	// Thanks: <https://learnopengl.com/Advanced-Lighting/Normal-Mapping>.
	mat3 m_normal = transpose(inverse(mat3(m_model)));
	vec3 T = normalize(m_normal * v_tangent.xyz);
	vec3 N = normalize(m_normal * v_norm);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	f_tbn = transpose(mat3(T, B, N));
	f_pos = vec3(m_model * vec4(v_pos, 1.0));
	f_norm = v_norm, f_uv = v_uv, f_color = v_color;
	gl_Position = mvp * vec4(v_pos, 1.0);
}
