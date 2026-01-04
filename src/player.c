#include <raylib.h>
#include <raymath.h>

#include "camera.h"
#include "game.h"
#include "player.h"
#include "terrain.h"

#define MOUSE_SENSITIVITY (9.f / 96.f)

#define WALK_SPEED (6.f)
#define WALK_ACCELERATION (24.f)
#define JUMP_IMPULSE (8.f)

static struct {
	Vector3 feet, linvel;
	float camera_pitch, camera_yaw, friction;
} player;

void player_restart() {
	player.feet = player.linvel = ORIGIN;
	player.feet.y = t_height(player.feet.x, player.feet.z);
	player.camera_pitch = player.camera_yaw = 0.f;
}

static float fsign(float x) {
	return x > EPSILON ? 1.f : x < -EPSILON ? -1.f : 0.f;
}

static void frick(float* axis, float amount) {
	const float old_sign = fsign(*axis);
	*axis -= amount;
	if (fsign(*axis) != old_sign)
		*axis = 0.f;
}

void player_update() {
	{
		const Vector2 dpos = GetMouseDelta();
		player.camera_pitch -= dpos.y * MOUSE_SENSITIVITY * GetFrameTime();
		player.camera_yaw -= dpos.x * MOUSE_SENSITIVITY * GetFrameTime();
	}

	player.camera_yaw = fmodf(player.camera_yaw, PI * 2.f);
	{
		const float max = PI * 0.4;
		player.camera_pitch = Clamp(player.camera_pitch, -max, max);
	}

	player.friction = 0.7f * (float)!IsKeyDown(KEY_LEFT_SHIFT);

	player.linvel.y -= GRAVITY / TICKRATE;
	const float stick_threshold = (GRAVITY * 1.1f) / TICKRATE, bottom = t_height(player.feet.x, player.feet.z);
	const bool on_ground = player.linvel.y < EPSILON && player.feet.y - bottom - stick_threshold < EPSILON;
	const Vector3 normal = t_norm(player.feet.x, player.feet.z);

	const float zdir = (float)IsKeyDown(KEY_W) - (float)IsKeyDown(KEY_S),
		    xdir = (float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A);
	if (fabsf(zdir) > EPSILON || fabsf(xdir) > EPSILON) {
		Vector3 forward = Vector3Scale(FORWARD, zdir), side = Vector3Scale(RIGHT, xdir);
		forward = Vector3RotateByAxisAngle(forward, UP, player.camera_yaw);
		side = Vector3RotateByAxisAngle(side, UP, player.camera_yaw);

		Vector3 absolute = Vector3Normalize(Vector3Add(forward, side));
		absolute = Vector3Scale(absolute, WALK_ACCELERATION / TICKRATE);
		if (Vector3LengthSqr(XYZ(player.linvel.x, 0.f, player.linvel.z)) < WALK_SPEED * WALK_SPEED)
			player.linvel = Vector3Add(player.linvel, absolute);
	}

	if (on_ground) {
		const float friction = normal.y * (player.friction * GRAVITY) / TICKRATE;
		player.feet.y = t_height(player.feet.x, player.feet.z), player.linvel.y = 0.f;

		if (IsKeyPressed(KEY_SPACE))
			player.linvel.y += JUMP_IMPULSE;
		else { // intentionally skip friction during jump
			const Vector3 lindir = Vector3Normalize(player.linvel);
			frick(&player.linvel.x, friction * lindir.x);
			frick(&player.linvel.z, friction * lindir.z);
		}
	}

	// clang-format off
	if (fabsf(player.linvel.x) < EPSILON) player.linvel.x = 0.f;
	if (fabsf(player.linvel.y) < EPSILON) player.linvel.y = 0.f;
	if (fabsf(player.linvel.z) < EPSILON) player.linvel.z = 0.f;
	// clang-format on

	player.feet = Vector3Add(player.feet, Vector3Scale(player.linvel, 1.f / TICKRATE));
	{
		Vector3 pos = player.feet, dir = Vector3RotateByAxisAngle(FORWARD, RIGHT, player.camera_pitch);
		pos.y += PLAYER_HEIGHT * 0.7f, dir = Vector3RotateByAxisAngle(dir, UP, player.camera_yaw);
		look_dir(pos, dir), look_up(UP);
	}
}

void player_draw() {
	DrawCapsule(player.feet, Vector3Add(player.feet, Vector3Scale(UP, PLAYER_HEIGHT)), PLAYER_RADIUS, 32, 32, RED);
}

void player_draw_debug_shit() {
	const int fs = 20;
	const char* deboog = TextFormat("V %2f", Vector3Length(player.linvel));
	DrawText(deboog, 5, GetScreenHeight() - 5 - fs, fs, WHITE);
}
