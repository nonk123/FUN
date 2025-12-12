#pragma once

#include <raylib.h>

#define MAX_CARS (32)

typedef struct {
	Model model;
	Vector2 offset, distance;
	float angle, angvel;
} Wheel;

typedef struct {
	float yaw, angvel;
	Vector2 pos, linvel;
	Model model;
	Wheel wheels[4];
	int id;
} Car;

void car_init(), car_teardown();
Car *spawn_car(float x, float z), *get_car(int);

Vector3 car_pos(const Car* car);
float car_pitch(const Car* car), car_roll(const Car* car);
void car_draw(const Car*);
