#include "object.h"
#include "obj_loader.h"
#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl_extensions.h"

object* object_new () {
	return object_new (NULL);
}

object* object_new (model* m) {
	object* result = (object*)malloc (sizeof (object));
	result -> m = m;
	result -> t = transform_new ();
	result -> t -> obj = result;

	return result;
}

void object_set_position (object* obj, glm::vec3 position) {
	transform_set_position (obj -> t, position);
}

void object_set_rotation (object* obj, glm::vec3 rotation) {
	transform_set_rotation (obj -> t, rotation);
}

void object_render (object* obj) {
	shader_use (obj -> m -> shader_id);
	glBindVertexArray (obj -> m -> VAO);

	shader_set_mat4 (obj -> m -> shader_id, "model", obj -> t -> model_matrix);
	model_render (obj -> m);

	glBindVertexArray (0);
}

void object_free (object* obj) {
	obj_delete (obj -> m -> obj);
	free (obj -> m);
	free (obj -> t);
}
