#include <raylib.h>
#include <raymath.h>

#include "player.h"
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
	player_draw();
	t_draw();
}
