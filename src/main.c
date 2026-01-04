#include <stdlib.h>

#include <raylib.h>

#include "game.h"
#include "light.h"
#include "log.h"
#include "player.h"
#include "raylibext.h"
#include "shader.h"
#include "skydome.h"
#include "terrain.h"
#include "vecmath.h"

int exitcode = EXIT_SUCCESS; // also used in `log.c`

typedef enum {
	LAYER_SKYDOME,
	LAYER_MAIN,
	LAYER_COUNT,
} Layer;

static RenderTexture2D layers[LAYER_COUNT] = {0};
static int last_w = 0, last_h = 0;

static void sync_layers() {
	const int new_w = GetScreenWidth(), new_h = GetScreenHeight();
	if (new_w == last_w && new_h == last_h)
		goto ok;
	for (Layer i = 0; i < LAYER_COUNT; i++) {
		UnloadRenderTexture(layers[i]);
		layers[i] = LoadRenderTexture(new_w, new_h);
	}
ok:
	last_w = new_w, last_h = new_h;
}

static void draw_game_fr() {
	extern Camera3D camera;

	ClearTransparent();
	BeginMode3D(camera);
	{
		sh_set(SHT_LEET, SHV_AMBIENT, RGBA(1.f, 1.f, 1.f, 0.3f), SHADER_UNIFORM_VEC4);
		light_begin();
		{
			/* light_pos(10, 7, 10); */
			/* light_color(1.f, 1.f, 1.f, 0.5f); */
			/* light_radius(10.f); */
			/* place_light(); */

			light_dir(0.8f, -0.5f, 0.1f);
			light_color(1.f, 1.f, 1.f, 0.7f);
			place_light();
		}
		light_end();
		game_draw();
	}
	EndMode3D();
}

static void compose_layers() {
	const Rectangle src = {0.f, 0.f, (float)last_w, (float)-last_h};
	ClearBackground(BLACK);
	for (int i = 0; i < LAYER_COUNT; i++)
		DrawTextureRec(layers[i].texture, src, XY(0.f, 0.f), WHITE);
	DrawFPS(5, 5);
	player_draw_debug_shit();
}

static void realmain() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "FUN."), InitAudioDevice();

	SetExitKey(KEY_BACKSPACE), DisableCursor();
	sh_init(), skydome_init(), t_init();
	game_restart();

	double last_time = GetTime(), ticks = 0.0;

	while (!WindowShouldClose()) {
		sync_layers();

		const double cur_time = GetTime();
		ticks += (cur_time - last_time) * TICKRATE;
		last_time = cur_time;

		if (IsKeyDown(KEY_R))
			game_restart(), ticks += 1.f;
		if (IsKeyPressed(KEY_K))
			MaximizeWindow();

		while (ticks >= 1.f)
			game_update(), ticks -= 1.f;

		BeginTextureMode(layers[LAYER_SKYDOME]);
		skydome_draw();
		EndTextureMode();

		BeginTextureMode(layers[LAYER_MAIN]);
		draw_game_fr();
		EndTextureMode();

		BeginDrawing();
		compose_layers();
		EndDrawing();
	}

	for (Layer i = 0; i < LAYER_COUNT; i++)
		UnloadRenderTexture(layers[i]);

	EnableCursor();
	t_teardown(), skydome_teardown(), sh_teardown();
	CloseAudioDevice(), CloseWindow();
}

int main(int argc, char* argv[]) {
	log_init();
	realmain();
	return exitcode;
}
