#version 100
precision mediump float;

varying vec3 f_pos;

void main() {
	vec4 horizonColor = vec4(0.6, 0.8, 1.0, 1.0), apexColor = vec4(0.1, 0.4, 0.8, 1.0);
	// gl_FragColor = mix(horizonColor, apexColor, f_pos.y);
	gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);
}
