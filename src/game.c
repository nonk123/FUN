#include <raylib.h>
#include <raymath.h>

#include "player.h"
#include "shader.h"
#include "terrain.h"

void restart() {
	t_restart();
	player_restart();
}

void game_update() {
	t_update();
	player_update();
}

void game_draw() {
	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

	float uv_scale = 1.f;
	sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
	player_draw();
	// TODO: draw more entities here.

	uv_scale = 2.f;
	sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
	t_draw();
}
