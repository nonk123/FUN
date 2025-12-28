#include <raylib.h>
#include <raymath.h>

#include "raylibext.h"
#include "shader.h"
#include "vecmath.h"

static Mesh mesh = {0};
static Material material = {0};

void skydome_init() {
	mesh = GenMeshPlane(64.f, 64.f, 16, 16);
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
	/* worthless.position = ORIGIN, worthless.target = Vector3Subtract(worthless.target, camera.position); */
	worthless.position = XYZ(0, 0, -1), worthless.target = ORIGIN;

	ClearTransparent();
	/* BeginMode3D(worthless); */
	DrawMesh(mesh, material, MatrixIdentity());
	/* EndMode3D(); */
}
