#include <raylib.h>
#include <raymath.h>

#include "camera.h"
#include "player.h"
#include "terrain.h"

#define MOUSE_SENSITIVITY (4.f / 96.f)

static struct {
	Vector3 feet;
	float body_angle;
	float camera_pitch, camera_yaw;
} player;

void player_restart() {
	player.feet = XYZ(0, 20, 0);
	player.feet.y = t_height(player.feet.x, player.feet.z);
	player.body_angle = player.camera_yaw = 0.f;
	player.camera_pitch = 0.f;
}

void player_update() {
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		const Vector2 dpos = GetMouseDelta();
		player.camera_pitch -= dpos.y * MOUSE_SENSITIVITY * GetFrameTime();
		player.camera_yaw += dpos.x * MOUSE_SENSITIVITY * GetFrameTime();
	}

	player.camera_yaw = fmodf(player.camera_yaw, PI * 2.f);
	{
		const float max = PI * 0.4;
		player.camera_pitch = Clamp(player.camera_pitch, -max, max);
	}

	look_dir(Vector3Add(player.feet, Vector3Scale(UP, PLAYER_HEIGHT * 0.7f)),
		Vector3RotateByAxisAngle(
			Vector3RotateByAxisAngle(FORWARD, RIGHT, player.camera_pitch), UP, player.camera_yaw));
	look_up(UP);
}

void player_draw() {
	DrawCapsule(player.feet, Vector3Add(player.feet, Vector3Scale(UP, PLAYER_HEIGHT)), PLAYER_RADIUS, 32, 32, RED);
}
