#include <stdlib.h>

#include <raylib.h>

#include "camera.h"
#include "car.h"
#include "light.h"
#include "log.h"
#include "shader.h"
#include "terrain.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`
static int player_car = 0;

static void realmain() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "FUN."), InitAudioDevice();

	SetExitKey(KEY_BACKSPACE);
	sh_init(), t_init(), car_init();

	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

	player_car = spawn_car(0.f, 0.f)->id;
	spawn_car(20.f, 0.f);
	spawn_car(20.f, 50.f);
	spawn_car(0.f, 50.f);

	Vector3 cam_pos = XYZ(10, 13, 10);
	const float speed = 30.f;

	while (!WindowShouldClose()) {
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

		{
			extern void reset_terrain();
			if (IsKeyPressed(KEY_R))
				reset_terrain();
			t_update();
		}

		BeginDrawing();
		ClearBackground(BLACK);
		{
			extern Camera3D camera;
			BeginMode3D(camera);
		}

		light_reset();
		{
			light_pos(10, 7, 10);
			light_color(1.f, 1.f, 1.f, 0.8f);
			light_radius(10.f);
			place_light();
		}
		light_done();

		sh_begin();
		{
			float uv_scale = 1.f;
			sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);

			for (int id = 1; id <= MAX_CARS; id++) {
				const Car* car = NULL;
				if ((car = get_car(id)))
					car_draw(car);
			}

			uv_scale = 2.f;
			sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
			t_draw();

			/* DrawCube(ORIGIN, 1.f, 1.f, 1.f, RED); */
			/* DrawCube(XYZ(4, 2, 1), 1.f, 1.f, 1.f, BLUE); */
		}
		sh_end();

		EndMode3D();
		DrawFPS(5, 5);
		EndDrawing();
	}

	car_teardown(), t_teardown(), sh_teardown();
	CloseAudioDevice(), CloseWindow();
}

int main(int argc, char* argv[]) {
	log_init();
	realmain();
	return exitcode;
}
