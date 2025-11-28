#include <stdlib.h>

#include <raylib.h>

#include "log.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

static void realmain() {
	InitWindow(800, 600, "FUN.");
	InitAudioDevice();

	SetTargetFPS(60);
	SetExitKey(KEY_BACKSPACE);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(5, 5);
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
