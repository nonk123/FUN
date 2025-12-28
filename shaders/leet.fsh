#version 100
precision mediump float;

#define MAX_LIGHTS (128)
#define LIGHT_DIRECTIONAL (0)
#define LIGHT_POINT (1)

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
