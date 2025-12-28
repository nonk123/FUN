#include <raylib.h>
#include <raymath.h>

#include "raylibext.h"

void DrawModelPro(Model model, Matrix transform, Color tint) {
	model.transform = MatrixMultiply(model.transform, transform);
	for (int i = 0; i < model.meshCount; i++) {
		Color color = model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color;

		Color colorTint = WHITE;
		colorTint.r = (unsigned char)((((float)color.r / 255.0f) * ((float)tint.r / 255.0f)) * 255.0f);
		colorTint.g = (unsigned char)((((float)color.g / 255.0f) * ((float)tint.g / 255.0f)) * 255.0f);
		colorTint.b = (unsigned char)((((float)color.b / 255.0f) * ((float)tint.b / 255.0f)) * 255.0f);
		colorTint.a = (unsigned char)((((float)color.a / 255.0f) * ((float)tint.a / 255.0f)) * 255.0f);

		model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = colorTint;
		DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], model.transform);
		model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = color;
	}
}

void ClearTransparent() {
	ClearBackground((Color){0, 0, 0, 0});
}
