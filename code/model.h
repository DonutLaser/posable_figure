#if !defined (MODEL_H)
#define MODEL_H

#include "obj_loader.h"
#include "glm/glm.hpp"

struct model {
	OBJ obj;
	glm::vec3 position;
	unsigned VAO;
	unsigned VBO;

	unsigned vertex_count;
	unsigned shader_id;
};

model model_new (OBJ obj, unsigned shader_id);
void model_render (model* m);

#endif