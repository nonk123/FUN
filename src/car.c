#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#include "assets.h"
#include "car.h"
#include "log.h"
#include "raylibext.h"
#include "shader.h"
#include "terrain.h"
#include "vecmath.h"

static Car* cars[MAX_CARS] = {0};

static Model model = {0};
static Material materials = {0};
static Texture2D palette = {0};

void car_init() {
	model = LoadModel(ASSETS "/car.obj");
	palette = LoadTexture(ASSETS "/palette.png");

	model.materialCount = 1, model.materials = make_material();
	model.materials->maps[MATERIAL_MAP_DIFFUSE].texture = palette;
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
		cars[i]->yaw = 0.f;
		cars[i]->model = model;
		return cars[i];
	}

	warn("SHIT RNA OUT OF CARS EXPLODING!!!");
	return NULL;
}

float car_pitch(const Car* car) {
	const float step = 0.2f;
	const Vector2 pos = XY(car->x, car->z), dir = Vector2Rotate(XY(0, -1), car->yaw),
		      dist = Vector2Scale(dir, step), front = Vector2Add(pos, dist);
	return atanf((t_height(front.x, front.y) - t_height(pos.x, pos.y)) / step);
}

void car_draw(const Car* car) {
	const Matrix trans = MatrixTranslate(car->x, t_height(car->x, car->z), car->z),
		     scale = MatrixScale(1.f, 1.f, 1.f), roty = MatrixRotateY(car->yaw),
		     rotx = MatrixRotateX(car_pitch(car)),
		     transform = MatrixMultiply(MatrixMultiply(MatrixMultiply(scale, rotx), roty), trans);
	DrawModelPro(car->model, transform, WHITE);
}

Car* get_car(int id) {
	expect(id >= 0 && id < MAX_CARS, "wtf is %d car", id);
	return cars[id];
}
