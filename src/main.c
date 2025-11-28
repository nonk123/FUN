#include <stdlib.h>

#include <raylib.h>

#include "camera.h"
#include "log.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

static void realmain() {
	extern Camera3D camera;

	InitWindow(800, 600, "FUN.");
	InitAudioDevice();

	SetTargetFPS(60);
	SetExitKey(KEY_BACKSPACE);

	look_at(XYZ(3, 3, -5), ORIGIN);

	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			ClearBackground(RAYWHITE);

			BeginMode3D(camera);
			{
				DrawCube(ORIGIN, 1.f, 1.f, 1.f, RED);
			}
			EndMode3D();

			DrawFPS(5, 5);
		}
		EndDrawing();
	}
}

static void cleanup() {
	CloseAudioDevice();
	CloseWindow();
}

int main(int argc, char* argv[]) {
	log_init();
	realmain();
	cleanup();
	return exitcode;
}
