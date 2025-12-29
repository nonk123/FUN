#version 100
precision mediump float;

varying vec3 f_pos;

void main() {
	vec3 horizonColor = vec3(0.6, 0.8, 1.0), apexColor = vec3(0.1, 0.4, 0.8);
	float factor = clamp(f_pos.y, 0.0, 1.0);
	gl_FragColor = vec4(mix(apexColor, horizonColor, factor), 1.0);
}
