#include <raylib.h>
#include <raymath.h>

#include "raylibext.h"
#include "shader.h"
#include "vecmath.h"

static Mesh mesh = {0};
static Material material = {0};

void skydome_init() {
	mesh = GenMeshHemiSphere(1.f, 128, 32);
	for (int i = 0; i < 3 * mesh.vertexCount; i++)
		mesh.normals[i] *= -1.f;

	material = LoadMaterialDefault();
	material.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	material.shader = shaders[SHT_SKYDOME];
}

void skydome_teardown() {
	UnloadMaterial(material);
	UnloadMesh(mesh);
}

void skydome_draw() {
	extern Camera3D camera;
	Camera3D worthless = camera;
	worthless.position = ORIGIN, worthless.target = Vector3Subtract(worthless.target, camera.position);

	ClearTransparent();
	BeginMode3D(worthless);
	{ DrawMesh(mesh, material, MatrixIdentity()); }
	EndMode3D();
}
