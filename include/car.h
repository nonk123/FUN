#pragma once

#include <raylib.h>

#define MAX_CARS (32)

typedef struct {
	float x, z, yaw;
	Model model;
	int id;
} Car;

void car_init(), car_teardown();
Car *spawn_car(float x, float z), *get_car(int);

float car_pitch(const Car* car);
void car_draw(const Car*);
