#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#include "assets.h"
#include "car.h"
#include "log.h"
#include "terrain.h"
#include "vecmath.h"

static Car* cars[MAX_CARS] = {0};

static Model model = {0};
static Texture2D palette = {0};

void car_init() {
	model = LoadModel(ASSETS "/car.obj");
	palette = LoadTexture(ASSETS "/palette.png");
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = palette;
}

void car_teardown() {
	UnloadTexture(palette);
	UnloadModel(model);
}

Car* spawn_car(float x, float z) {
	for (int i = 0; i < MAX_CARS; i++) {
		if (cars[i])
			continue;
		cars[i] = MemAlloc(sizeof(Car));
		cars[i]->id = i, cars[i]->x = x, cars[i]->z = z;
		cars[i]->model = model;
		return cars[i];
	}

	warn("SHIT RNA OUT OF CARS EXPLODING!!!");
	return NULL;
}

void car_draw(const Car* car) {
	const Vector3 scale = XYZ(1, 1, 1), pos = XYZ(car->x, t_height(car->x, car->z), car->z);
	DrawModelEx(car->model, pos, UP, 0.f, scale, WHITE);
}

Car* get_car(int id) {
	expect(id >= 0 && id < MAX_CARS, "wtf is %d car", id);
	return cars[id];
}
