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

model model_new (OBJ obj, material mat);
void model_render (model* m);

#endif