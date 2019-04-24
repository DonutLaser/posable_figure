#include "collider.h"

collider* collider_new (glm::vec3 position, float radius) {
	collider* result = (collider*)malloc (sizeof (collider));
	result -> position = position;
	result -> radius = radius;

	return result;
}
