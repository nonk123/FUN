#include <stdlib.h>
#include <time.h>

#include <open-simplex-noise.h>
#include <raylib.h>
#include <raymath.h>

#include "assets.h"
#include "shader.h"
#include "terrain.h"
#include "vecmath.h"

/// Points per side of a chunk.
#define RESOLUTION (16)

/// Length of a chunk's side.
#define SIDE (16.f)

/// Coordinate downscaling factor for noise computation.
#define SCALE (20.f)

/// Another scaling factor for the second noise function.
#define OCTAVE (2.1f)

/// (Temporary) heightmap magnitude.
#define STEEPNESS (8.f)

/// How many chunks to keep loaded around the camera.
#define VIEW_RADIUS (5)

typedef struct Chunk {
	int64_t x, z;
	Model model;
	struct Chunk* next;
} Chunk;

static struct osn_context* osn = NULL;

static Chunk* root = NULL;
static void nuke_chunk(Chunk*);

static Texture2D green_grass = {0};

static void kill_terrain() {
	while (root)
		nuke_chunk(root);
	if (osn) {
		open_simplex_noise_free(osn);
		osn = NULL;
	}
}

void reset_terrain() { // used in `game.c`
	static int64_t last_seed = 0;
	const int64_t new_seed = time(NULL);
	if (new_seed != last_seed) {
		last_seed = new_seed;
		kill_terrain();
		open_simplex_noise(new_seed, &osn);
		t_update();
	}
}

void t_init() {
	reset_terrain();
	green_grass = LoadTexture(ASSETS "/Grass_A_BaseColor.png");
}

void t_teardown() {
	UnloadTexture(green_grass);
	kill_terrain();
}

static Vector2 chunk_center(const Chunk* ch) {
	Vector2 c = {0};
	c.x = floorf((float)ch->x * SIDE);
	c.y = floorf((float)ch->z * SIDE);
	return c;
}

static float noise_at(float x, float z, int octave) {
	const float octaves[2] = {1.f, OCTAVE}, scale = octaves[octave] * SCALE;
	x /= scale, z /= scale;
	return 1.f + 0.5f * open_simplex_noise2(osn, x, z);
}

float t_height(float x, float z) {
	return (noise_at(x, z, 0) * noise_at(x, z, 1) - 1.f) * STEEPNESS;
}

static float c_x(const Chunk* c, int64_t x) {
	return ((float)(c->x * RESOLUTION + x) / RESOLUTION) * SIDE;
}

static float c_z(const Chunk* c, int64_t z) {
	return ((float)(c->z * RESOLUTION + z) / RESOLUTION) * SIDE;
}

Vector3 t_norm(float x, float z) {
	const float step = SIDE / (float)RESOLUTION;

	Vector3 norm = XYZ(0, 2, 0);
	norm.x = t_height(x - step, z) - t_height(x + step, z);
	norm.z = t_height(x, z - step) - t_height(x, z + step);
	return Vector3Normalize(norm);
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
	const int64_t x = (int64_t)(wx / SIDE), z = (int64_t)(wz / SIDE);
	for (const Chunk* c = root; c; c = c->next)
		if (x == c->x && z == c->z)
			return true;
	return false;
}

static void generate_vert(const Chunk* c, size_t idx, int64_t x, int64_t z) {
	Vector3 *vertices = (Vector3*)c->model.meshes->vertices, *norms = (Vector3*)c->model.meshes->normals;
	Vector2* texcoords = (Vector2*)c->model.meshes->texcoords;
	Color* colors = (Color*)c->model.meshes->colors;

	vertices[idx] = XYZ((float)x / RESOLUTION * SIDE, t_height(c_x(c, x), c_z(c, z)), (float)z / RESOLUTION * SIDE);
	texcoords[idx] = XY((float)x / RESOLUTION, (float)z / RESOLUTION);
	norms[idx] = t_norm(c_x(c, x), c_z(c, z));
	colors[idx] = WHITE;
}

static void generate_chunk(float x, float z) {
	Chunk* c = MemAlloc(sizeof(*c));
	c->next = root, root = c;

	c->x = (int64_t)(x / SIDE);
	c->z = (int64_t)(z / SIDE);

	Mesh* mesh = MemAlloc(sizeof(*mesh));
	c->model.meshCount = 1, c->model.meshes = mesh;
	c->model.transform = MatrixIdentity();

	c->model.materialCount = 1;
	c->model.materials = MemAlloc(sizeof(Material) * c->model.materialCount);

	c->model.materials = make_material();
	c->model.materials->maps[MATERIAL_MAP_ALBEDO].texture = green_grass;

	c->model.meshMaterial = MemAlloc(sizeof(int));
	*c->model.meshMaterial = 0;

	mesh->triangleCount = 2 * RESOLUTION * RESOLUTION;
	mesh->vertexCount = 3 * mesh->triangleCount;

	mesh->vertices = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->normals = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->texcoords = MemAlloc(2 * sizeof(float) * mesh->vertexCount);
	mesh->colors = MemAlloc(4 * mesh->vertexCount);

	size_t i = 0;
	for (int z = 0; z < RESOLUTION; z++)
		for (int x = 0; x < RESOLUTION; x++) {
			generate_vert(c, i++, x, z);
			generate_vert(c, i++, x + 1, z + 1);
			generate_vert(c, i++, x + 1, z);

			generate_vert(c, i++, x, z);
			generate_vert(c, i++, x, z + 1);
			generate_vert(c, i++, x + 1, z + 1);
		}

	UploadMesh(mesh, false);
}

static int64_t i64abs(int64_t x) {
	return x < 0 ? -x : x;
}

void t_update() {
	extern Camera3D camera;
	const Vector2 centerf = {.x = camera.position.x, .y = camera.position.z};
	const int64_t centerx = (int64_t)(floorf(centerf.x) / SIDE), centerz = (int64_t)(floorf(centerf.y) / SIDE);

	for (Chunk* c = root; c; c = c->next)
		if (i64abs(c->x - centerx) > VIEW_RADIUS || i64abs(c->z - centerz) > VIEW_RADIUS)
			nuke_chunk(c);

	for (int64_t ix = -VIEW_RADIUS; ix <= VIEW_RADIUS; ix++)
		for (int64_t iz = -VIEW_RADIUS; iz <= VIEW_RADIUS; iz++) {
			if (i64abs(centerx - ix) > VIEW_RADIUS || i64abs(centerz - iz) > VIEW_RADIUS)
				continue;
			const float x = centerf.x + (float)ix * SIDE, z = centerf.y + (float)iz * SIDE;
			if (chunk_exists(x, z))
				continue;
			generate_chunk(x, z);
		}
}

void t_draw() {
	for (const Chunk* c = root; c; c = c->next) {
		Vector3 pos = {.x = SIDE * (float)c->x, .y = 0.f, .z = SIDE * (float)c->z};
		DrawModel(c->model, pos, 1.f, WHITE);
	}
}
