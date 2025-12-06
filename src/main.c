#include <stdlib.h>

#include <raylib.h>

#include "car.h"
#include "game.h"
#include "light.h"
#include "log.h"
#include "shader.h"
#include "terrain.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

static void realmain() {
	extern void game_update(), game_draw(), restart();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "FUN."), InitAudioDevice();

	SetExitKey(KEY_BACKSPACE);
	sh_init(), t_init(), car_init();

	restart();

	const double tick_delay = 1.0 / TICKRATE;
	double last_time = GetTime(), ticks = 0.0;

	while (!WindowShouldClose()) {
		const double cur_time = GetTime();
		ticks += (cur_time - last_time) * TICKRATE;
		last_time = cur_time;

		if (IsKeyPressed(KEY_R))
			restart(), ticks += 1.f;

		while (ticks >= 1.f)
			game_update(), ticks -= 1.f;

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
		game_draw();
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
