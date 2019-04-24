#if !defined (COLLIDER_H)
#define COLLIDER_H

#include "glm/glm.hpp"

struct collider {
	glm::vec3 position;
	float radius;
};

collider* collider_new (glm::vec3  position, float radius);

#endif