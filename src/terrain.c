#include <stdlib.h>
#include <string.h>

#include <open-simplex-noise.h>
#include <raylib.h>
#include <raymath.h>

#include "log.h"
#include "terrain.h"
#include "vecmath.h"

static struct osn_context* osn = NULL;

/// Points per side of a chunk.
#define RESOLUTION (16)

/// Length of a chunk's side.
#define SIDE (16.f)

/// (Temporary) heightmap magnitude.
#define STEEPNESS (8.f)

/// Maximum distance to a chunk's center before unloading.
#define VIEW_RADIUS (64.f)

typedef struct Chunk {
	int64_t x, z;
	Model model;
	struct Chunk* next;
} Chunk;

static Chunk* root = NULL;
static void nuke_chunk(Chunk*);

void t_init() {
	open_simplex_noise(0, &osn);
}

void t_teardown() {
	while (root)
		nuke_chunk(root);

	open_simplex_noise_free(osn);
	osn = NULL;
}

static Vector2 chunk_center(const Chunk* ch) {
	Vector2 c = {0};
	c.x = (float)ch->x * SIDE;
	c.y = (float)ch->z * SIDE;
	return c;
}

static float pos_noise(float x, float z) {
	return 1.f + 0.5f * open_simplex_noise2(osn, x, z);
}

static float c_height(const Chunk* c, int64_t x, int64_t z) {
	const float fx = (float)(c->x * RESOLUTION + x) / RESOLUTION * SIDE;
	const float fz = (float)(c->z * RESOLUTION + z) / RESOLUTION * SIDE;
	return -pos_noise(fx, fz) * STEEPNESS;
}

static void nuke_chunk(Chunk* target) {
	Chunk* prev = NULL;
	for (Chunk* c = root; c; c = c->next)
		if (c->next == target) {
			prev = c;
			break;
		}

	if (prev)
		prev->next = target->next;
	else // prolly nuking root
		root = target->next;

	UnloadModel(target->model);
	target->next = NULL;
	MemFree(target);
}

static bool chunk_exists(float wx, float wz) {
	const int64_t x = (int64_t)roundf(wx / SIDE), z = (int64_t)roundf(wz / SIDE);
	for (const Chunk* c = root; c; c = c->next)
		if (x == c->x && z == c->z)
			return true;
	return false;
}

static void maybe_generate_chunk(float x, float z) {
	if (chunk_exists(x, z))
		return;

	Chunk* c = MemAlloc(sizeof(*c));
	c->next = root, root = c;

	c->x = (int64_t)roundf(x / SIDE);
	c->z = (int64_t)roundf(z / SIDE);

	Mesh* mesh = MemAlloc(sizeof(*mesh));
	c->model.meshCount = 1, c->model.meshes = mesh;
	c->model.transform = MatrixTranslate(x, 0.f, z);

	c->model.materialCount = 1;
	c->model.materials = MemAlloc(sizeof(Material));

	Material material = LoadMaterialDefault();
	// TODO: do stuff with `material`?
	c->model.materials[0] = material;

	c->model.meshMaterial = MemAlloc(sizeof(int));
	c->model.meshMaterial[0] = 0;

	mesh->triangleCount = 2 * (RESOLUTION + 1) * (RESOLUTION + 1);
	mesh->vertexCount = 3 * mesh->triangleCount;

	mesh->vertices = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->normals = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->texcoords = MemAlloc(2 * sizeof(float) * mesh->vertexCount);

	int i = 0;

#define Ht(_x, _z) XYZ(chunk_center(c).x, c_height(c, (_x), (_z)), chunk_center(c).y)
#define Nm(_x, _z)                                                                                                     \
	Vector3Normalize(XYZ(c_height(c, (_x) + 1, (_z)) - c_height(c, (_x) - 1, (_z)), 2.f,                           \
		c_height(c, (_x), (_z) + 1) - c_height(c, (_x), (_z) - 1)))
#define Tx(_x, _y) XY((float)(_x) / RESOLUTION, (float)(_y) / RESOLUTION)

#define Full(d, _x, _z)                                                                                                \
	vertices[i * 6 + (d)] = Ht((_x), (_z));                                                                        \
	norms[i * 6 + (d)] = Nm((_x), (_z));                                                                           \
	texcoords[i * 6 + (d)] = Tx((_x), (_z));

	Vector3 *vertices = (Vector3*)mesh->vertices, *norms = (Vector3*)mesh->normals;
	Vector2* texcoords = (Vector2*)mesh->texcoords;

	for (int x = 0; x < RESOLUTION; x++)
		for (int z = 0; z < RESOLUTION; z++) {
			Full(0, x, z);
			Full(1, x + 1, z + 1);
			Full(2, x + 1, z);

			Full(3, x, z);
			Full(4, x, z + 1);
			Full(5, x + 1, z + 1);

			i++;
		}

	UploadMesh(mesh, false);

#undef Full

#undef Tx
#undef Nm
#undef Ht
}

void t_update() {
	extern Camera3D camera;
	const Vector2 center = {.x = camera.position.x, .y = camera.position.z};

	for (Chunk* c = root; c; c = c->next)
		if (Vector2Distance(chunk_center(c), center) > VIEW_RADIUS - EPSILON)
			nuke_chunk(c);

	const float r = VIEW_RADIUS;
	for (float x = center.x - r; fabsf(x - center.x - r) > EPSILON; x += SIDE)
		for (float z = center.y - r; fabsf(z - center.y - r) > EPSILON; z += SIDE)
			if (Vector2Distance(XY(x, z), center) < VIEW_RADIUS + EPSILON)
				maybe_generate_chunk(x, z);
}

void t_draw() {
	for (const Chunk* c = root; c; c = c->next)
		DrawModel(c->model, Vector3Zero(), 1.f, GREEN);
}
