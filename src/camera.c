#include "camera.h"
#include "vecmath.h"

void set_lock_cursor(bool lock) {
	if (lock)
		DisableCursor();
	else
		EnableCursor();
}

Camera3D camera = {0}; // extern in `main.c`, `terrain.c`, `skydome.c`

void look_at(Vector3 from, Vector3 to) {
	camera.projection = CAMERA_PERSPECTIVE, camera.fovy = 90.f, camera.up = UP;
	camera.position = from, camera.target = to;
}

void look_dir(Vector3 from, Vector3 dir) {
	look_at(from, Vector3Add(from, dir));
}

void look_up(Vector3 up) {
	camera.up = up;
}
