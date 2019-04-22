#if !defined (MODEL_H)
#define MODEL_H

#include "obj_loader.h"
#include "glm/glm.hpp"

struct model {
	OBJ obj;

	glm::vec3 world_position;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
	float bounding_sphere_radius;

	model* parent;

	unsigned VAO;
	unsigned VBO;

	unsigned vertex_count;
	unsigned shader_id;

	bool visible;

	glm::vec3 multiply_color;
};

model* model_new (OBJ obj, unsigned shader_id);

void model_set_parent (model* main, model* parent);
void model_set_position (model* m, glm::vec3 position);

void model_render (model* m);

#endif