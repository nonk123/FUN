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

		Vector3 back = XYZ(0, 1.5f, -12.f);
		back = Vector3RotateByAxisAngle(back, RIGHT, car_pitch(c));
		back = Vector3RotateByAxisAngle(back, UP, c->yaw);
		cam_pos = Vector3Add(car_pos(c), back);

		Vector3 dir = XYZ(0.f, 0.f, 1.f);
		dir = Vector3RotateByAxisAngle(dir, RIGHT, car_pitch(c));
		dir = Vector3RotateByAxisAngle(dir, UP, c->yaw);

		look_dir(cam_pos, dir);
		look_up(Vector3RotateByAxisAngle(UP, FORWARD, car_roll(c)));
	} else {
		look_dir(cam_pos, XYZ(-1, -1, -1));
		look_up(UP);
	}

	t_update();

	for (int id = 1; id <= MAX_CARS; id++) {
		Car* car = NULL;
		if (!(car = get_car(id)))
			continue;

		if (id == player_car) {
			extern void car_thrust(Car*, float);
			car_thrust(car, (float)(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)));

			extern void car_steer(Car*, float);
			car_steer(car, (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)));
		}

		extern void car_update(Car*);
		car_update(car);
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
