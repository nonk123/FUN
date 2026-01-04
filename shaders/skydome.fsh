#version 330

in vec3 f_pos;
out vec4 final_color;

void main() {
	vec3 horizonColor = vec3(0.6, 0.8, 1.0), apexColor = vec3(0.1, 0.5, 0.9);
	float factor = clamp(f_pos.y, 0.0, 1.0);
	final_color = vec4(mix(horizonColor, apexColor, factor), 1.0);
}
