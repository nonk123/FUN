#pragma once

#include <raylib.h>

#define MAX_CARS (32)

typedef struct {
	float x, z;
	Model model;
	int id;
} Car;

void car_init(), car_teardown();
Car* spawn_car(float x, float z), *  get_car(int);
void car_draw(const Car*);
