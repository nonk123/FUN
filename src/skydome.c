#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "raylibext.h"
#include "shader.h"

static Model model = {0};

void skydome_init() {
	model.meshCount = 1, model.meshes = MemAlloc(sizeof(*model.meshes));
	model.materialCount = 1, model.materials = MemAlloc(sizeof(*model.materials));
	model.meshMaterial = MemAlloc(sizeof(model.meshMaterial)), model.meshMaterial[0] = 0;
	model.materials->maps = MemAlloc(sizeof(*model.materials->maps));
	model.materials->maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	model.materials->shader = shaders[SHT_SKYDOME];
	model.transform = MatrixIdentity();

	Mesh *mesh = model.meshes, tmpmesh = GenMeshSphere(1.f, 32, 32);
	mesh->vertexCount = tmpmesh.vertexCount, mesh->triangleCount = tmpmesh.triangleCount;
	mesh->vertices = MemAlloc((size_t)mesh->vertexCount * 3 * sizeof(float));
	memcpy(mesh->vertices, tmpmesh.vertices, (size_t)mesh->vertexCount * 3 * sizeof(float));
	mesh->normals = MemAlloc((size_t)mesh->vertexCount * 3 * sizeof(float));
	memcpy(mesh->normals, tmpmesh.normals, (size_t)mesh->vertexCount * 3 * sizeof(float));
	mesh->texcoords = MemAlloc((size_t)mesh->vertexCount * 2 * sizeof(float));
	memcpy(mesh->texcoords, tmpmesh.texcoords, (size_t)mesh->vertexCount * 2 * sizeof(float));

	Vector3* vertices = (Vector3*)mesh->vertices;
	for (int i = 0; i < mesh->triangleCount; i++) {
		Vector3 tmp = vertices[i * 3 + 2];
		vertices[i * 3 + 2] = vertices[i * 3 + 0];
		vertices[i * 3 + 0] = tmp;
	}

	for (int i = 0; i < 3 * mesh->vertexCount; i++)
		mesh->normals[i] *= -1.f;

	UnloadMesh(tmpmesh), UploadMesh(mesh, false);
}

void skydome_teardown() {
	UnloadModel(model);
}

void skydome_draw() {
	extern Camera3D camera;
	ClearTransparent();
	BeginMode3D(camera);
	DrawModel(model, camera.position, 1.f, WHITE);
	EndMode3D();
}
