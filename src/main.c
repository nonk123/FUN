#include <stdlib.h>

#include <raylib.h>

#include "camera.h"
#include "log.h"
#include "shader.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

static void realmain() {
	extern Camera3D camera;

	InitWindow(800, 600, "FUN.");
	InitAudioDevice();

	shader_init();

	SetTargetFPS(60);
	SetExitKey(KEY_BACKSPACE);

	look_at(XYZ(3, 3, -5), ORIGIN);

	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			ClearBackground(RAYWHITE);

			BeginMode3D(camera);
			shader_begin();

			DrawCube(ORIGIN, 1.f, 1.f, 1.f, WHITE);

			shader_end();
			EndMode3D();

			DrawFPS(5, 5);
		}
		EndDrawing();
	}

	shader_teardown();

	CloseAudioDevice();
	CloseWindow();
}

int main(int argc, char* argv[]) {
	log_init();
	realmain();
	return exitcode;
}
