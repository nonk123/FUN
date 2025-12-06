#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#include "camera.h"
#include "car.h"
#include "shader.h"
#include "terrain.h"

static int player_car = 0;
static Vector3 cam_pos = XYZ(10, 13, 10);

void restart() {
	extern void reset_terrain();
	reset_terrain();

	extern void nuke_cars();
	nuke_cars();

	player_car = spawn_car(0.f, 0.f)->id;
	spawn_car(20.f, 0.f);
	spawn_car(20.f, 50.f);
	spawn_car(0.f, 50.f);
}

void game_update() {
	if (player_car) {
		const Car* c = get_car(player_car);
		cam_pos = XYZ(c->x, 0.f, c->z);
		Vector3 back = XYZ(0, 0, 12.f);
		back = Vector3RotateByAxisAngle(back, UP, c->yaw);
		cam_pos = Vector3Add(cam_pos, back);
		cam_pos.y = t_height(cam_pos.x, cam_pos.z) + 2.f;
		look_at(cam_pos, XYZ(c->x, t_height(c->x, c->z) + 0.6, c->z));
	} else {
		look_at(cam_pos, XYZ(-1, -1, -1));
	}

	t_update();

	for (int id = 1; id <= MAX_CARS; id++) {
		Car* car = NULL;
		if (!(car = get_car(id)))
			continue;
		if (id == player_car) {
			Vector2 dir = {0};
			dir.x = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
			dir.y = (float)(IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
			car_control(car, dir);
		}
	}
}

void game_draw() {
	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

	float uv_scale = 1.f;
	sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);

	for (int id = 1; id <= MAX_CARS; id++) {
		Car* car = NULL;
		if (!(car = get_car(id)))
			continue;
		car_draw(car);
	}

	uv_scale = 2.f;
	sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
	t_draw();
}
