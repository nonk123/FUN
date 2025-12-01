#include <stdlib.h>

#include <raylib.h>

#include "camera.h"
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
			ClearBackground(RAYWHITE);

			BeginMode3D(camera);
			sh_begin();

			sh_set(SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.2f), SHADER_UNIFORM_VEC4);

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
