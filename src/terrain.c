#include <stdlib.h>
#include <time.h>

#include <open-simplex-noise.h>
#include <raylib.h>
#include <raymath.h>

#include "assets.h"
#include "shader.h"
#include "terrain.h"
#include "vecmath.h"

static struct osn_context* osn = NULL;

/// Points per side of a chunk.
#define RESOLUTION (16)

/// Length of a chunk's side.
#define SIDE (16.f)

/// Coordinate downscaling factor for noise computation.
#define SCALE (20.f)

/// Another scaling factor for the second noise function.
#define OCTAVE (2.1f)

/// (Temporary) heightmap magnitude.
#define STEEPNESS (12.f)

/// Maximum distance to a chunk's center before unloading.
#define VIEW_RADIUS (128.f)

typedef struct Chunk {
	int64_t x, z;
	Model model;
	struct Chunk* next;
} Chunk;

static Chunk* root = NULL;
static void nuke_chunk(Chunk*);

static Texture2D green_grass = {0};

void t_init() {
	open_simplex_noise(time(NULL), &osn);
	green_grass = LoadTexture(ASSETS "/green-grass.png");
}

void t_teardown() {
	UnloadTexture(green_grass);
	while (root)
		nuke_chunk(root);
	open_simplex_noise_free(osn);
	osn = NULL;
}

static Vector2 chunk_center(const Chunk* ch) {
	Vector2 c = {0};
	c.x = roundf((float)ch->x * SIDE);
	c.y = roundf((float)ch->z * SIDE);
	return c;
}

static float pos_noise(float x, float z, int octave) {
	const float octaves[2] = {1.f, OCTAVE}, scale = octaves[octave] * SCALE;
	x /= scale, z /= scale;
	return 1.f + 0.5f * open_simplex_noise2(osn, x, z);
}

float t_height(float x, float z) {
	return (pos_noise(x, z, 0) * pos_noise(x, z, 1) - 1.f) * STEEPNESS;
}

static float c_x(const Chunk* c, int64_t x) {
	return ((float)(c->x * RESOLUTION + x) / RESOLUTION) * SIDE;
}

static float c_z(const Chunk* c, int64_t z) {
	return ((float)(c->z * RESOLUTION + z) / RESOLUTION) * SIDE;
}

static float c_height(const Chunk* c, int64_t x, int64_t z) {
	return t_height(c_x(c, x), c_z(c, z));
}

Vector3 t_norm(float x, float z) {
	const float step = SIDE / (float)RESOLUTION;

	Vector3 norm = XYZ(0, 2, 0);
	norm.x = t_height(x - step, z) - t_height(x + step, z);
	norm.z = t_height(x, z - step) - t_height(x, z + step);
	return Vector3Normalize(norm);
}

static Vector3 c_norm(const Chunk* c, int64_t x, int64_t z) {
	return t_norm(c_x(c, x), c_z(c, z));
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

static void generate_chunk(float x, float z) {
	Chunk* c = MemAlloc(sizeof(*c));
	c->next = root, root = c;

	c->x = (int64_t)roundf(x / SIDE);
	c->z = (int64_t)roundf(z / SIDE);

	Mesh* mesh = MemAlloc(sizeof(*mesh));
	c->model.meshCount = 1, c->model.meshes = mesh;
	c->model.transform = MatrixIdentity();

	c->model.materialCount = 1;
	c->model.materials = MemAlloc(sizeof(Material) * c->model.materialCount);

	Material material = make_material();
	material.maps[MATERIAL_MAP_ALBEDO].texture = green_grass;
	c->model.materials[0] = material;

	c->model.meshMaterial = MemAlloc(sizeof(int));
	c->model.meshMaterial[0] = 0;

	mesh->triangleCount = 2 * RESOLUTION * RESOLUTION;
	mesh->vertexCount = 3 * mesh->triangleCount;

	mesh->vertices = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->normals = MemAlloc(3 * sizeof(float) * mesh->vertexCount);
	mesh->texcoords = MemAlloc(2 * sizeof(float) * mesh->vertexCount);
	mesh->colors = MemAlloc(4 * mesh->vertexCount);

#define Vert(d, _x, _z)                                                                                                \
	do {                                                                                                           \
		const size_t _i = i + (d);                                                                             \
		vertices[_i] = XYZ(                                                                                    \
			(float)(_x) / RESOLUTION * SIDE, c_height(c, (_x), (_z)), (float)(_z) / RESOLUTION * SIDE);    \
		norms[_i] = c_norm(c, (_x), (_z));                                                                     \
		texcoords[_i] = XY((float)(_x) / RESOLUTION, (float)(_z) / RESOLUTION);                                \
		colors[_i] = WHITE;                                                                                    \
	} while (0)

	Vector3 *vertices = (Vector3*)mesh->vertices, *norms = (Vector3*)mesh->normals;
	Vector2* texcoords = (Vector2*)mesh->texcoords;
	Color* colors = (Color*)mesh->colors;

	size_t i = 0;

	for (int z = 0; z < RESOLUTION; z++)
		for (int x = 0; x < RESOLUTION; x++) {
			Vert(0, x, z);
			Vert(1, x + 1, z + 1);
			Vert(2, x + 1, z);

			Vert(3, x, z);
			Vert(4, x, z + 1);
			Vert(5, x + 1, z + 1);

			i += 6;
		}

	UploadMesh(mesh, false);

#undef Vert
}

void t_update() {
	extern Camera3D camera;
	const Vector2 center = {.x = camera.position.x, .y = camera.position.z};

	for (Chunk* c = root; c; c = c->next)
		if (Vector2Distance(chunk_center(c), center) - VIEW_RADIUS > EPSILON)
			nuke_chunk(c);

	const int idim = (int)roundf((2.f * VIEW_RADIUS) / SIDE);
	for (int ix = 0; ix < idim; ix++)
		for (int iz = 0; iz < idim; iz++) {
			const float x = center.x - VIEW_RADIUS + (float)ix * SIDE,
				    z = center.y - VIEW_RADIUS + (float)iz * SIDE;
			if (Vector2Distance(XY(x, z), center) - VIEW_RADIUS > -EPSILON)
				continue;
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
