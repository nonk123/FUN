#include <stdlib.h>

#include <raylib.h>

#include "camera.h"
#include "light.h"
#include "log.h"
#include "shader.h"
#include "terrain.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

static void realmain() {
	extern Camera3D camera;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "FUN.");
	InitAudioDevice();

	SetExitKey(KEY_BACKSPACE);
	sh_init();

	sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

	while (!WindowShouldClose()) {
		look_dir(XYZ(10, 10, 10), XYZ(-1, -1, -1));
		t_update();

		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(camera);
		{
			light_reset();
			{
				light_pos(10, 7, -10);
				light_color(1.f, 1.f, 1.f, 0.8f);
				place_light();
			}
			light_done();

			sh_begin();
			{
				// t_draw();
				DrawCube(ORIGIN, 1.f, 1.f, 1.f, RED);
				DrawCube(XYZ(4, 2, 1), 1.f, 1.f, 1.f, BLUE);
			}
			sh_end();
		}
		EndMode3D();
		DrawFPS(5, 5);
		EndDrawing();
	}

	sh_teardown();

	CloseAudioDevice();
	CloseWindow();
}

int main(int argc, char* argv[]) {
	log_init(), t_init();
	realmain();
	t_teardown();
	return exitcode;
}
