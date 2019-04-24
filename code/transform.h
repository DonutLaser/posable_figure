#if !defined (TRANSFORM_H)
#define TRANSFORM_H

#include "glm/glm.hpp"
// #include "object.h"

struct transform;
struct object;

struct transform_children {
	transform** data;
	unsigned count;
	unsigned capacity;
};

struct transform {
	glm::mat4 model_matrix;

	glm::vec3 position;
	glm::vec3 world_position;
	glm::vec3 rotation;
	glm::vec3 world_rotation;
	glm::vec3 scale;

	transform* parent;
	transform_children children;
	object* obj;
};

transform* transform_new ();
transform* transform_new (glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

void transform_set_parent (transform* child, transform* parent);
void transform_set_position (transform* t, glm::vec3 position);
void transform_set_rotation (transform* t, glm::vec3 rotation);
void transform_set_scale (transform* t, glm::vec3 scale);

glm::vec3 transform_get_world_position (transform* t);

#endif