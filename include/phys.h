#pragma once

#include <stdbool.h>

#include <raylib.h>

/// Axis-aligned capsule.
typedef struct {
	Vector3 position;
	float length, radius;
} Capsule;

bool collide_capsule_to_triangle(Capsule, Vector3[3]);
bool collide_capsule_to_terrain(Capsule);
