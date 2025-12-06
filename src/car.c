#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#include "assets.h"
#include "car.h"
#include "game.h"
#include "log.h"
#include "raylibext.h"
#include "shader.h"
#include "terrain.h"
#include "vecmath.h"

#define WHEEL_RADIUS (0.5f)
#define WHEEL_DISTANCE XY(0.9f, 1.1f)

static Car* cars[MAX_CARS] = {0};

static Model car_model = {0}, wheel_model = {0};
static Material materials = {0};
static Texture2D palette = {0};

static void setup_model(Model* model) {
	model->materialCount = 1, model->materials = make_material();
	model->materials->maps[MATERIAL_MAP_DIFFUSE].texture = palette;
}

void car_init() {
	car_model = LoadModel(ASSETS "/car.obj");
	wheel_model = LoadModel(ASSETS "/wheel.obj");
	palette = LoadTexture(ASSETS "/palette.png");
	setup_model(&car_model), setup_model(&wheel_model);
}

void nuke_cars() { // used in `game.c`
	for (int idx = 0; idx < MAX_CARS; idx++) {
		if (!cars[idx])
			continue;
		MemFree(cars[idx]), cars[idx] = NULL;
	}
}

void car_teardown() {
	nuke_cars();
	UnloadTexture(palette);
	UnloadModel(car_model);
}

Car* spawn_car(float x, float z) {
	for (int idx = 0; idx < MAX_CARS; idx++) {
		if (cars[idx])
			continue;
		cars[idx] = MemAlloc(sizeof(Car));
		cars[idx]->id = idx + 1, cars[idx]->x = x, cars[idx]->z = z;
		cars[idx]->yaw = 0.f, cars[idx]->linvel = XY(0, 0), cars[idx]->torque = 0.f;
		cars[idx]->models[CARMODEL_HULL] = car_model;
		for (int j = 0; j < 4; j++)
			cars[idx]->models[CARMODEL_WHEELS + j] = wheel_model;
		return cars[idx];
	}

	warn("SHIT RNA OUT OF CARS EXPLODING!!!");
	return NULL;
}

static float car_angle_at(const Car* car, Vector2 dist, float step) {
	dist = Vector2Rotate(Vector2Multiply(dist, WHEEL_DISTANCE), car->yaw);
	const Vector2 pos = XY(car->x, car->z), end = Vector2Add(pos, dist);
	return atan2f(-step, (t_height(end.x, end.y) - t_height(pos.x, pos.y)));
}

static float ang_avg4(float angs[4]) {
	float x = 0.f, y = 0.f;
	for (int i = 0; i < 4; i++)
		x += cosf(angs[i]), y += sinf(angs[i]);
	return atan2f(y / 4.f, x / 4.f);
}

float car_pitch(const Car* car) {
	const float fl = car_angle_at(car, XY(-1, 1), -WHEEL_DISTANCE.y),
		    fr = car_angle_at(car, XY(1, 1), -WHEEL_DISTANCE.y),
		    bl = car_angle_at(car, XY(-1, -1), WHEEL_DISTANCE.y),
		    br = car_angle_at(car, XY(1, -1), WHEEL_DISTANCE.y);
	return ang_avg4((float[4]){fl, fr, bl, br});
}

float car_roll(const Car* car) {
	const float fl = car_angle_at(car, XY(-1, 1), -WHEEL_DISTANCE.x),
		    fr = car_angle_at(car, XY(1, 1), WHEEL_DISTANCE.x),
		    bl = car_angle_at(car, XY(-1, -1), -WHEEL_DISTANCE.x),
		    br = car_angle_at(car, XY(1, -1), WHEEL_DISTANCE.x);
	return ang_avg4((float[4]){fl, fr, bl, br});
}

void car_control(Car* car, Vector2 dir) {
	// BIG TODO.
	const float speed = 10.f / TICKRATE;
	const Vector2 thrust = Vector2Scale(Vector2Rotate(XY(0, -dir.y), car->yaw), speed);
	car->x += thrust.x, car->z += thrust.y;
}

void car_draw(const Car* car) {
	const Matrix transl = MatrixTranslate(car->x, t_height(car->x, car->z) + WHEEL_RADIUS, car->z),
		     scale = MatrixScale(1.f, 1.f, 1.f), roty = MatrixRotateY(car->yaw),
		     rotx = MatrixRotateX(car_pitch(car)), rotz = MatrixRotateZ(car_roll(car)),
		     rotations = MatrixMultiply(MatrixMultiply(MatrixMultiply(scale, rotz), rotx), roty),
		     car_transform = MatrixMultiply(rotations, transl);
	DrawModelPro(car->models[CARMODEL_HULL], car_transform, WHITE);

	const Vector2 offs[4] = {XY(-1, 1), XY(1, 1), XY(-1, -1), XY(1, -1)};
	for (int i = 0; i < 4; i++) {
		const float x = offs[i].x * WHEEL_DISTANCE.x, z = offs[i].y * WHEEL_DISTANCE.y;
		const Matrix transform = MatrixMultiply(MatrixTranslate(x, 0.f, z), car_transform);
		DrawModelPro(car->models[CARMODEL_WHEELS + i], transform, WHITE);
	}
}

Car* get_car(int id) {
	expect(id > 0 && id <= MAX_CARS, "wtf is %d car", id);
	return cars[id - 1];
}
