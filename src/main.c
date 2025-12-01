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

	InitWindow(800, 600, "FUN.");
	InitAudioDevice();

	sh_init();

	SetTargetFPS(60);
	SetExitKey(KEY_BACKSPACE);

	while (!WindowShouldClose()) {
		look_at(XYZ(3, 3, -5), ORIGIN);
		t_update();

		BeginDrawing();
		{
			ClearBackground(BLACK);

			BeginMode3D(camera);
			sh_begin();

			light_reset();

			light_pos(1.f, 2.f, -3.f);
			light_color(1.f, 1.f, 1.f, 0.5f);
			light_place();

			light_done();

			t_draw();
			DrawCube(ORIGIN, 1.f, 1.f, 1.f, RED);

			sh_end();
			EndMode3D();

			DrawFPS(5, 5);
		}
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
