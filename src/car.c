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
		cars[idx]->id = idx + 1, cars[idx]->pos = Vector2Zero();
		cars[idx]->yaw = 0.f, cars[idx]->linvel = XY(0, 0), cars[idx]->torque = 0.f;
		cars[idx]->models[CARMODEL_HULL] = car_model;
		for (int j = 0; j < 4; j++)
			cars[idx]->models[CARMODEL_WHEELS + j] = wheel_model;
		return cars[idx];
	}

	warn("SHIT RNA OUT OF CARS EXPLODING!!!");
	return NULL;
}

static Matrix car_mat(const Car* car) {
	const Matrix transl = MatrixTranslate(car->pos.x, t_height(car->pos.x, car->pos.y) + WHEEL_RADIUS, car->pos.y),
		     scale = MatrixScale(1.f, 1.f, 1.f), roty = MatrixRotateY(car->yaw),
		     rotx = MatrixRotateX(car_pitch(car)), rotz = MatrixRotateZ(car_roll(car)),
		     rotations = MatrixMultiply(MatrixMultiply(MatrixMultiply(scale, rotz), rotx), roty);
	return MatrixMultiply(rotations, transl);
}

static float wheel_grip(const Car* car, Vector2 offset) {
	const Matrix mat = car_mat(car);
	offset = Vector2Multiply(offset, WHEEL_DISTANCE);
	const Vector3 absolute = Vector3Transform(XYZ(offset.x, 0.f, offset.y), mat);
	const float diff = fabsf(t_height(absolute.x, absolute.z) - absolute.y);
	if (diff > WHEEL_RADIUS)
		return 0.f;
	else if (diff < 0.5f * WHEEL_RADIUS)
		return 1.f;
	else
		return diff / WHEEL_RADIUS;
}

static float angle_at_wheel(const Car* car, Vector2 dist, float step) {
	dist = Vector2Rotate(Vector2Multiply(dist, WHEEL_DISTANCE), car->yaw);
	const Vector2 end = Vector2Add(car->pos, dist);
	return atanf((t_height(end.x, end.y) - t_height(car->pos.x, car->pos.y)) / step);
}

static float ang_avg(float angs[4], int count) {
	float x = 0.f, y = 0.f;
	for (int i = 0; i < count; i++)
		x += cosf(angs[i]), y += sinf(angs[i]);
	return atanf(y / x);
}

Vector3 car_pos(const Car* car) {
	return XYZ(car->pos.x, t_height(car->pos.x, car->pos.y), car->pos.y);
}

float car_pitch(const Car* car) {
	const float fl = angle_at_wheel(car, XY(-1, 1), WHEEL_DISTANCE.y),
		    fr = angle_at_wheel(car, XY(1, 1), WHEEL_DISTANCE.y),
		    bl = angle_at_wheel(car, XY(-1, -1), -WHEEL_DISTANCE.y),
		    br = angle_at_wheel(car, XY(1, -1), -WHEEL_DISTANCE.y);
	return -ang_avg((float[4]){fl, fr, bl, br}, 4);
}

float car_roll(const Car* car) {
	const float fl = angle_at_wheel(car, XY(-1, 1), -WHEEL_DISTANCE.x),
		    fr = angle_at_wheel(car, XY(1, 1), WHEEL_DISTANCE.x),
		    bl = angle_at_wheel(car, XY(-1, -1), -WHEEL_DISTANCE.x),
		    br = angle_at_wheel(car, XY(1, -1), WHEEL_DISTANCE.x);
	return ang_avg((float[4]){fl, fr, bl, br}, 4);
}

static const Vector2 wheel_offs[4] = {XY(-1, 1), XY(1, 1), XY(-1, -1), XY(1, -1)};

void car_control(Car* car, Vector2 dir) {
	// BIG TODO.
	const float speed = 10.f / TICKRATE;
	const Vector2 thrust = Vector2Scale(Vector2Rotate(XY(0, -dir.y), car->yaw), speed);
	for (int i = 0; i < 4; i++) {
		car->linvel = Vector2Zero();
		car->torque += 0.f;
	}
}

void car_update(Car* car) {
	car->pos = Vector2Add(car->pos, Vector2Scale(car->linvel, 1.f / TICKRATE));
	car->yaw += car->torque / TICKRATE;
}

void car_draw(const Car* car) {
	const Matrix car_transform = car_mat(car);
	DrawModelPro(car->models[CARMODEL_HULL], car_transform, WHITE);

	for (int i = 0; i < 4; i++) {
		const float x = wheel_offs[i].x * WHEEL_DISTANCE.x, z = wheel_offs[i].y * WHEEL_DISTANCE.y;
		const Matrix transform = MatrixMultiply(MatrixTranslate(x, 0.f, z), car_transform);
		DrawModelPro(car->models[CARMODEL_WHEELS + i], transform, WHITE);
	}
}

Car* get_car(int id) {
	expect(id > 0 && id <= MAX_CARS, "wtf is %d car", id);
	return cars[id - 1];
}
