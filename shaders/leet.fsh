#version 330

#define MAX_LIGHTS (%d)
#define LIGHT_DIRECTIONAL (1)
#define LIGHT_POINT (2)

in vec3 f_pos;
in vec2 f_uv;
in vec3 f_norm;
in vec4 f_color;
in mat3 f_tbn;

uniform sampler2D albedo, normal_map;
uniform vec4 ambient;

uniform struct {
	int type;
	vec3 pos, aux;
	vec4 color;
} lights[MAX_LIGHTS];

out vec4 final_color;

void main() {
	vec4 surface_color = f_color * texture(albedo, f_uv);

	vec3 normal = texture(normal_map, f_uv).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(normal * f_tbn);

	vec4 light_color = ambient;
	for (int i = 0; i < MAX_LIGHTS; i++) {
		float factor = 1.0;
		vec3 light_dist = vec3(0.0);

		if (lights[i].type == LIGHT_DIRECTIONAL) {
			light_dist = -lights[i].aux;
		} else if (lights[i].type == LIGHT_POINT) {
			light_dist = lights[i].pos - f_pos;
			factor *= clamp(lights[i].aux.x / length(light_dist), 0.0, 1.0);
		} else {
			continue;
		}

		factor *= dot(normal, normalize(light_dist));
		light_color += lights[i].color * max(0.0, factor);
	}

	light_color = min(light_color, vec4(1.0));
	final_color = light_color * surface_color;

	vec3 shadowed = mix(vec3(0.0), final_color.rgb, final_color.a);
	final_color = vec4(shadowed, surface_color.a);
	final_color = min(final_color, vec4(1.0));
}
