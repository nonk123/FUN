#include <raylib.h>
#include <raymath.h>

#include "player.h"
#include "shader.h"
#include "terrain.h"

void game_restart() {
	t_restart();
	player_restart();
}

void game_update() {
	t_update();
	player_update();
}

void game_draw() {
	sh_set(SHT_LEET, SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.4f), SHADER_UNIFORM_VEC4);
	player_draw();
	t_draw();
}
