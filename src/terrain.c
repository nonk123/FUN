#include <stdlib.h>
#include <time.h>

#include <open-simplex-noise.h>
#include <raylib.h>
#include <raymath.h>

#include "assets.h"
#include "game.h"
#include "shader.h"
#include "terrain.h"
#include "vecmath.h"

/// How many seconds to generate chunks for before moving on with the
/// rest of the game logic.
#define GENERATION_PERIOD (0.8 / TICKRATE)

/// How many chunks to spread a complete grass texture over.
#define UV_SCALE (3)

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

void t_restart() { // used in `game.c`
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
	t_restart();
	green_grass = LoadTexture(ASSETS "/Grass_A_BaseColor.png");
}

void t_teardown() {
	UnloadTexture(green_grass), kill_terrain();
}

static Vector2 chunk_center(const Chunk* ch) {
	return XY(roundf((float)ch->x * SIDE), roundf((float)ch->z * SIDE));
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
	const int64_t x = (int64_t)(roundf(wx) / SIDE), z = (int64_t)(roundf(wz) / SIDE);
	for (const Chunk* c = root; c; c = c->next)
		if (x == c->x && z == c->z)
			return true;
	return false;
}

static float txbruh(int64_t a, float b) {
	return (b + (float)(a % UV_SCALE)) / UV_SCALE;
}

static void generate_chunk(float x, float z) {
	Chunk* c = MemAlloc(sizeof(*c));
	c->next = root, root = c, c->x = (int64_t)(roundf(x) / SIDE), c->z = (int64_t)(roundf(z) / SIDE);

	Mesh* mesh = MemAlloc(sizeof(*mesh));
	c->model.meshCount = 1, c->model.meshes = mesh;
	c->model.transform = MatrixIdentity();

	c->model.materialCount = 1, c->model.materials = make_leet_materials();
	c->model.materials->maps[MATERIAL_MAP_ALBEDO].texture = green_grass;
	c->model.meshMaterial = MemAlloc(sizeof(int)), c->model.meshMaterial[0] = 0;

	mesh->vertexCount = (RESOLUTION + 1) * (RESOLUTION + 1);
	mesh->triangleCount = 2 * RESOLUTION * RESOLUTION;

	mesh->indices = MemAlloc(3 * sizeof(uint16_t) * mesh->triangleCount);
	mesh->vertices = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->normals = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->texcoords = MemAlloc(2 * sizeof(float) * mesh->vertexCount);
	mesh->colors = MemAlloc(4 * mesh->vertexCount);

	Vector3 *vertices = (Vector3*)c->model.meshes->vertices, *norms = (Vector3*)c->model.meshes->normals;
	Vector2* texcoords = (Vector2*)c->model.meshes->texcoords;
	Color* colors = (Color*)c->model.meshes->colors;

	size_t v = 0;
	for (int z = 0; z <= RESOLUTION; z++)
		for (int x = 0; x <= RESOLUTION; x++) {
			const float x01 = (float)x / RESOLUTION, z01 = (float)z / RESOLUTION;
			vertices[v] = XYZ(x01 * SIDE, t_height(c_x(c, x), c_z(c, z)), z01 * SIDE);
			texcoords[v] = XY(txbruh(c->x, x01), txbruh(c->z, z01));
			norms[v] = t_norm(c_x(c, x), c_z(c, z)), colors[v] = WHITE;
			v += 1;
		}

	size_t i = 0;
	for (int z = 0; z < RESOLUTION; z++)
		for (int x = 0; x < RESOLUTION; x++) {
			const uint16_t v = x + z * (RESOLUTION + 1);

			mesh->indices[i++] = v;
			mesh->indices[i++] = v + RESOLUTION + 2;
			mesh->indices[i++] = v + 1;

			mesh->indices[i++] = v;
			mesh->indices[i++] = v + RESOLUTION + 1;
			mesh->indices[i++] = v + RESOLUTION + 2;
		}

	UploadMesh(mesh, false);
}

void t_update() {
	extern Camera3D camera;
	const Vector2 centerf = XY(camera.position.x, camera.position.z);
	const int64_t centerx = (int64_t)(roundf(centerf.x) / SIDE), centerz = (int64_t)(roundf(centerf.y) / SIDE);

	for (Chunk* c = root; c; c = c->next)
		if (llabs(c->x - centerx) > VIEW_RADIUS || llabs(c->z - centerz) > VIEW_RADIUS)
			nuke_chunk(c);

	const double start = GetTime();
	for (int64_t ix = -VIEW_RADIUS; ix <= VIEW_RADIUS; ix++)
		for (int64_t iz = -VIEW_RADIUS; iz <= VIEW_RADIUS; iz++) {
			if (llabs(centerx - ix) > VIEW_RADIUS || llabs(centerz - iz) > VIEW_RADIUS)
				continue;
			const float x = centerf.x + (float)ix * SIDE, z = centerf.y + (float)iz * SIDE;
			if (chunk_exists(x, z))
				continue;
			generate_chunk(x, z);
			if (GetTime() - start > GENERATION_PERIOD)
				return;
		}
}

void t_draw() {
	for (const Chunk* c = root; c; c = c->next) {
		Vector3 pos = XYZ(c_x(c, 0), 0.f, c_z(c, 0));
		DrawModel(c->model, pos, 1.f, WHITE);
	}
}
