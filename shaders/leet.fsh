#version 100
precision mediump float;

#define MAX_LIGHTS (128)
#define LIGHT_DIRECTIONAL (0)
#define LIGHT_POINT (1)

varying vec3 f_pos;
varying vec2 f_uv;
varying vec3 f_norm;
varying vec4 f_color;
varying mat3 f_tbn;

uniform sampler2D albedo, normal_map;
uniform vec4 ambient;

uniform int light_count;
uniform struct {
	int type;
	vec3 position, aux;
	vec4 color;
} lights[MAX_LIGHTS];

void main() {
	vec4 texel_color = texture2D(albedo, f_uv);
	vec4 surface_color = f_color * texel_color;

	vec4 light_color = ambient;
	for (int i = 0; i < light_count; i++) {
		float diffuse = 0.0;

		vec3 normal = texture2D(normal_map, f_uv).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(f_tbn * normal);

		if (lights[i].type == LIGHT_DIRECTIONAL) {
			vec3 light_dir = normalize(-lights[i].aux);
			diffuse = dot(normal, light_dir);
		} else if (lights[i].type == LIGHT_POINT) {
			vec3 light_dist = lights[i].position - f_pos;
			float factor = clamp(lights[i].aux.x / length(light_dist), 0.0, 1.0);
			vec3 light_dir = normalize(light_dist);
			diffuse = factor * dot(normal, light_dir);
		}

		light_color += lights[i].color * max(0.0, diffuse);
	}

	light_color = min(light_color, vec4(1.0));
	vec4 final_color = light_color * surface_color;

	vec3 shadowed = mix(vec3(0.0), final_color.rgb, final_color.a);
	final_color = vec4(shadowed, surface_color.a);
	gl_FragColor = min(final_color, vec4(1.0));
}
