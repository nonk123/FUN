#pragma once

#include <raylib.h>

#define MAX_CARS (32)

enum {
	CARMODEL_HULL,
	CARMODEL_WHEELS,
	CARMODEL_WHEEL_FL = CARMODEL_WHEELS,
	CARMODEL_WHEEL_FR,
	CARMODEL_WHEEL_BL,
	CARMODEL_WHEEL_BR,
	CARMODEL_COUNT,
};

typedef struct {
	float yaw, torque;
	Vector2 pos, linvel;
	Model models[CARMODEL_COUNT];
	int id;
} Car;

void car_init(), car_teardown();
Car *spawn_car(float x, float z), *get_car(int);

Vector3 car_pos(const Car* car);
float car_pitch(const Car* car), car_roll(const Car* car);
void car_draw(const Car*);

void car_control(Car*, Vector2);
