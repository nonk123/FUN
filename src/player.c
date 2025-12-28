#include <raylib.h>
#include <raymath.h>

#include "camera.h"
#include "game.h"
#include "player.h"
#include "terrain.h"

#define MOUSE_SENSITIVITY (9.f / 96.f)
#define JUMP_IMPULSE (8.f)

static struct {
	Vector3 feet, linvel;
	float camera_pitch, camera_yaw;
} player;

void player_restart() {
	player.feet = XYZ(0, 20, 0);
	player.feet.y = t_height(player.feet.x, player.feet.z);
	player.camera_pitch = player.camera_yaw = 0.f;
}

static float fsign(float x) {
	return x < 0.f ? -1.f : 1.f;
}

static void setabs(float* out, float abs) {
	*out = (float)(abs >= EPSILON) * fsign(*out) * fabsf(abs);
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

	const float zdir = (float)IsKeyDown(KEY_W) - (float)IsKeyDown(KEY_S),
		    xdir = (float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A);
	if (fabsf(zdir) > EPSILON || fabsf(xdir) > EPSILON) {
		Vector3 forward = Vector3Scale(FORWARD, zdir), side = Vector3Scale(RIGHT, xdir);
		forward = Vector3RotateByAxisAngle(forward, UP, player.camera_yaw);
		side = Vector3RotateByAxisAngle(side, UP, player.camera_yaw);

		Vector3 absolute = Vector3Add(forward, side);
		absolute = Vector3Scale(Vector3Normalize(absolute), 5.f);
		player.linvel.x = absolute.x, player.linvel.z = absolute.z;
	}

	player.linvel.y -= GRAVITY / TICKRATE;
	player.feet = Vector3Add(player.feet, Vector3Scale(player.linvel, 1.f / TICKRATE));

	const float stick_threshold = 0.03f, bottom = t_height(player.feet.x, player.feet.z);
	if (player.feet.y <= bottom + stick_threshold) {
		player.feet.y = bottom, player.linvel.y = 0.f;
		const float friction = 4.f / TICKRATE;

		if (IsKeyPressed(KEY_SPACE))
			player.linvel.y += JUMP_IMPULSE;
		else {
			setabs(&player.linvel.x, fabsf(player.linvel.x) - friction);
			setabs(&player.linvel.z, fabsf(player.linvel.z) - friction);
		}
	}

	{
		Vector3 pos = player.feet, dir = Vector3RotateByAxisAngle(FORWARD, RIGHT, player.camera_pitch);
		pos.y += PLAYER_HEIGHT * 0.7f, dir = Vector3RotateByAxisAngle(dir, UP, player.camera_yaw);
		look_dir(pos, dir), look_up(UP);
	}
}

void player_draw() {
	DrawCapsule(player.feet, Vector3Add(player.feet, Vector3Scale(UP, PLAYER_HEIGHT)), PLAYER_RADIUS, 32, 32, RED);
}
