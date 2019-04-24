#include "transform.h"

#include "glm/gtc/matrix_transform.hpp"

// Children array
static bool children_array_child_exists (transform_children* array, transform* child) {
	for (unsigned i = 0; i < array -> count; ++i) {
		if (array -> data[i] == child)
			return true;
	}

	return false;
}

static transform_children children_array_new () {
	transform_children result = { };
	result.capacity = 2;
	result.data = (transform**)malloc (sizeof (transform*) * result.capacity);

	return result;
}

static void children_array_add (transform_children* array, transform* child) {
	if (array -> count == array -> capacity) {
		array -> capacity *= 2;
		array -> data = (transform**)realloc (array -> data, sizeof (transform*) * array -> capacity);
	}

	if (!children_array_child_exists (array, child))
		array -> data[array -> count++] = child;
}

static void children_array_free (transform_children* array) {
	free (array -> data);
}

// Children stack
static transform* children_array_pop (transform_children* array) {
	--array -> count;
	return array -> data[array -> count];
}

static glm::mat4 generate_model_matrix (transform* t) {
	glm::mat4 result = glm::mat4 (1.0f);
	result = glm::translate (result, t -> world_position);
	result = glm::rotate (result, glm::radians (t -> rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
	result = glm::rotate (result, glm::radians (t -> rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
	result = glm::rotate (result, glm::radians (t -> rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
	result = glm::scale (result, t -> scale);

	return result;
}

static void update_child (transform* child) {
	child -> world_position = child -> position;

	child -> model_matrix = generate_model_matrix (child);
	child -> model_matrix = child -> parent -> model_matrix * child -> model_matrix;

	for (unsigned i = 0; i < child -> children.count; ++i)
		update_child (child -> children.data[i]);
}

static void update_children (transform* t) {
	t -> world_position = t -> position;
	t -> world_rotation = t -> rotation;

	t -> model_matrix = generate_model_matrix (t);

	if (t -> parent)
		t -> model_matrix = t -> parent -> model_matrix * t -> model_matrix;

	for (unsigned i = 0; i < t -> children.count; ++i)
		update_child (t -> children.data[i]);
}

transform* transform_new () {
	return transform_new (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (1.0f, 1.0f, 1.0f));
}

transform* transform_new (glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	transform* result = (transform*)malloc (sizeof (transform));
	result -> position = position;
	result -> rotation = rotation;
	result -> scale = scale;

	result -> world_position = position;
	result -> world_rotation = rotation;

	result -> parent = NULL;
	result -> children = children_array_new ();

	result -> model_matrix = glm::mat4 (1.0f);
	result -> model_matrix = glm::translate (result -> model_matrix, result -> world_position);
	result -> model_matrix = glm::rotate (result -> model_matrix, glm::radians (rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
	result -> model_matrix = glm::rotate (result -> model_matrix, glm::radians (rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
	result -> model_matrix = glm::rotate (result -> model_matrix, glm::radians (rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
	result -> model_matrix = glm::scale (result -> model_matrix, result -> scale);

	return result;
}

void transform_set_parent (transform* child, transform* parent) {
	child -> parent = parent;	
	children_array_add (&parent -> children, child);
	update_children (child);
}

void transform_set_position (transform* t, glm::vec3 position) {
	t -> position = position;
	update_children (t);
}

void transform_set_rotation (transform* t, glm::vec3 rotation) {
	t -> rotation = rotation;
	update_children (t);
}

void transform_set_scale (transform* t, glm::vec3 scale) {
	t -> scale = scale;
	update_children (t);
}

glm::vec3 transform_get_world_position (transform* t) {
	if (t -> parent) {
		glm::vec4 pos = t -> parent -> model_matrix * glm::vec4 (t -> position, 1.0f);
		return glm::vec3 (pos.x, pos.y, pos.z);
	}
	
	return t -> position;
}
