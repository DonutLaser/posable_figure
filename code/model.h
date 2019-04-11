#if !defined (MODEL_H)
#define MODEL_H

#include "material.h"
#include "obj_loader.h"

struct model {
	OBJ obj;
	glm::vec3 position;
	unsigned VAO;
	unsigned VBO;

	unsigned vertex_count;
	material mat;
};

model model_new (float* vertices, unsigned vert_count, float* uvs, unsigned uv_count, float* normals, unsigned normal_count, material mat);
void model_render (model* m);

#endif