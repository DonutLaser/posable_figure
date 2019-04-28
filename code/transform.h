#if !defined (TRANSFORM_H)
#define TRANSFORM_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct object;

struct transform {
	glm::mat4 model_matrix;

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 forward;

	transform* parent;
	object* obj;
};

transform* transform_new ();
transform* transform_new (glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

void transform_set_parent (transform* child, transform* parent);
void transform_set_position (transform* t, glm::vec3 position);
void transform_rotate (transform* t, glm::vec3 axis, float angle);
void transform_set_scale (transform* t, glm::vec3 scale);
void transform_set_rotation (transform* t, glm::quat rotation);

glm::mat4 transform_get_world_matrix (transform* t);
glm::vec3 transform_get_world_position (transform* t);
glm::quat transform_get_world_rotation (transform* t);

#endif