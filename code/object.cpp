#include "object.h"
#include "obj_loader.h"
#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl_extensions.h"

static void update_bounding_boxes (object* obj) {
	obj -> c -> position = transform_get_world_position (obj -> t);

	for (unsigned i = 0; i < obj -> t -> children.count; ++i)
		update_bounding_boxes (obj -> t -> children.data[i] -> obj);
}

object* object_new () {
	return object_new (NULL);
}

object* object_new (model* m) {
	object* result = (object*)malloc (sizeof (object));
	result -> m = m;
	result -> t = transform_new ();
	result -> t -> obj = result;
	result -> c = collider_new (result -> t -> world_position, 1.0f);

	return result;
}

void object_set_position (object* obj, glm::vec3 position) {
	transform_set_position (obj -> t, position);
	obj -> c -> position = transform_get_world_position (obj -> t);
}

void object_set_rotation (object* obj, glm::vec3 rotation) {
	transform_set_rotation (obj -> t, rotation);

	update_bounding_boxes (obj);
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
	free (obj -> c);
}
