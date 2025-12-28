#include <raylib.h>
#include <raymath.h>

#include "camera.h"
#include "shader.h"
#include "terrain.h"

static Vector3 player_feet = ORIGIN;
static Vector3 cam_pos = XYZ(10, 13, 10);

void restart() {
	extern void reset_terrain();
	reset_terrain();

	player_feet = XYZ(0, 20, 0);
	player_feet.y = t_height(player_feet.x, player_feet.z);
}

void game_update() {
	look_dir(cam_pos, XYZ(-1, -1, -1));
	look_up(UP);
	t_update();
}

void game_draw() {
	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

	float uv_scale = 1.f;
	sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
	// TODO: draw entities here.

	uv_scale = 2.f;
	sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
	t_draw();
}
