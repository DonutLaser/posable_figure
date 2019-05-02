#include "transform.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

static glm::mat4 generate_model_matrix (transform* t) {
	glm::mat4 result = glm::mat4 (1.0f);
	result = glm::translate (result, t -> position);
	result = result * glm::toMat4 (t -> rotation);
	result = glm::scale (result, t -> scale);

	return result;
}

transform* transform_new () {
	return transform_new (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (1.0f, 1.0f, 1.0f));
}

transform* transform_new (glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	transform* result = (transform*)malloc (sizeof (transform));
	result -> position = position;
	result -> rotation = glm::quat (rotation);
	result -> scale = scale;

	result -> parent = NULL;

	result -> model_matrix = generate_model_matrix (result);

	result -> right = glm::vec3 (result -> model_matrix[0][0], result -> model_matrix[1][0], result -> model_matrix[2][0]);
	result -> up = glm::vec3 (result -> model_matrix[0][1], result -> model_matrix[1][1], result -> model_matrix[2][1]);
	result -> forward = glm::vec3 (result -> model_matrix[0][2], result -> model_matrix[1][2], result -> model_matrix[2][2]);
	
	return result;
}

void transform_set_parent (transform* child, transform* parent) {
	child -> parent = parent;	
}

void transform_set_position (transform* t, glm::vec3 position) {
	t -> position = position;
	t -> model_matrix = generate_model_matrix (t);
}

void transform_rotate (transform* t, glm::vec3 axis, float angle) {
	glm::quat rotation = glm::angleAxis (glm::radians (angle), axis);
	t -> rotation = t -> rotation * rotation;
	transform_set_rotation (t, t -> rotation);
}

void transform_set_scale (transform* t, glm::vec3 scale) {
	t -> scale = scale;
	t -> model_matrix = generate_model_matrix (t);
}

void transform_set_rotation (transform* t, glm::quat rotation) {
	t -> rotation = rotation;

	t -> model_matrix = generate_model_matrix (t);

	t -> right = glm::vec3 (t -> model_matrix[0][0], t -> model_matrix[1][0], t -> model_matrix[2][0]);
	t -> up = glm::vec3 (t -> model_matrix[0][1], t -> model_matrix[1][1], t -> model_matrix[2][1]);
	t -> forward = glm::vec3 (t -> model_matrix[0][2], t -> model_matrix[1][2], t -> model_matrix[2][2]);
}

glm::mat4 transform_get_world_matrix (transform* t) {
	glm::mat4 result = t -> model_matrix;
	transform* parent = t -> parent;
	while (parent) {
		result = parent -> model_matrix * result;
		parent = parent -> parent;
	}

	return result;
}

glm::vec3 transform_get_world_position (transform* t) {
	if (t -> parent) {
		glm::mat4 parent_matrix = transform_get_world_matrix (t -> parent);
		glm::vec4 pos = parent_matrix * glm::vec4 (t -> position, 1.0f);
		return glm::vec3 (pos.x, pos.y, pos.z);
	}
	
	return t -> position;
}

glm::quat transform_get_world_rotation (transform* t) {
	glm::quat result = t -> rotation;
	transform* parent = t -> parent;
	while (parent) {
		result = parent -> rotation * result;
		parent = parent -> parent;
	}

	return result;
}