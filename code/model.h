#if !defined (MODEL_H)
#define MODEL_H

#include "obj_loader.h"
#include "glm/glm.hpp"

struct model {
	OBJ obj;
	glm::vec3 position;
	glm::vec3 rotation;
	float angle;
	model* parent;
	unsigned VAO;
	unsigned VBO;

	unsigned vertex_count;
	unsigned shader_id;

	bool visible;
};

model* model_new (OBJ obj, unsigned shader_id);
void model_render (model* m);

#endif