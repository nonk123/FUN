#include <stdlib.h>

#include <raylib.h>

#include "camera.h"
#include "light.h"
#include "log.h"
#include "shader.h"
#include "terrain.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

static void realmain() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "FUN."), InitAudioDevice();

	SetExitKey(KEY_BACKSPACE);
	sh_init(), t_init();

	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

	Vector3 pos = XYZ(10, 13, 10);
	const float speed = 30.f;

	while (!WindowShouldClose()) {
		{
			Vector3 dir = ORIGIN;
			dir.x += (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
			dir.z += (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));
			dir = Vector3Normalize(dir);
			pos = Vector3Add(pos, Vector3Scale(dir, speed * GetFrameTime()));
		}

		look_dir(pos, XYZ(-1, -1, -1));
		t_update();

		BeginDrawing();
		ClearBackground(BLACK);
		{
			extern Camera3D camera;
			BeginMode3D(camera);
		}

		light_reset();
		{
			light_pos(10, 7, -10);
			light_color(1.f, 1.f, 1.f, 0.8f);
			light_radius(10.f);
			place_light();
		}
		light_done();

		sh_begin();
		{
			float uv_scale = 2.f;
			sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);
			t_draw();

			uv_scale = 1.f;
			sh_set(SHV_UV_SCALE, &uv_scale, SHADER_UNIFORM_FLOAT);

			/* DrawCube(ORIGIN, 1.f, 1.f, 1.f, RED); */
			/* DrawCube(XYZ(4, 2, 1), 1.f, 1.f, 1.f, BLUE); */
		}
		sh_end();

		EndMode3D();
		DrawFPS(5, 5);
		EndDrawing();
	}

	t_teardown(), sh_teardown();
	CloseAudioDevice(), CloseWindow();
}

int main(int argc, char* argv[]) {
	log_init();
	realmain();
	return exitcode;
}
