#include "model.h"
#include "shader.h"

#include "gl_extensions.h"
#include <gl/gl.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static void recalculate_world_position (model* m) {
	m -> world_position = m -> position;
	model* root = m -> parent;
	while (root) {
		m -> world_position += root -> position;
		root = root -> parent;
	}
}

model* model_new (OBJ obj, unsigned shader_id) {
	model* result = (model*)malloc (sizeof (model));
	result -> obj = obj;
	result -> vertex_count = obj.vertices.count / 3;
	result -> position = glm::vec3 (0.0f, 0.0f, 0.0f);
	result -> world_position = result -> position;
	result -> rotation = glm::vec3 (0.0f, 0.0f, 0.0f);
	result -> scale = 1.0f; 
	result -> shader_id = shader_id;
	result -> visible = true;
	result -> parent = NULL;
	result -> bounding_sphere_radius = 0.1f;
	result -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);

	glGenVertexArrays (1, &result -> VAO);
	glGenBuffers (1, &result -> VBO);

	glBindVertexArray (result -> VAO);

	glBindBuffer (GL_ARRAY_BUFFER, result -> VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (float) * (obj.vertices.count + obj.uv.count + obj.normals.count), NULL, GL_STATIC_DRAW);

	glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof (float) * obj.vertices.count, obj.vertices.data);
	glBufferSubData (GL_ARRAY_BUFFER, sizeof (float) * obj.vertices.count, sizeof (float) * obj.uv.count, obj.uv.data);
	glBufferSubData (GL_ARRAY_BUFFER, sizeof (float) * (obj.vertices.count + obj.uv.count), sizeof (float) * obj.normals.count, obj.normals.data);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (void*)(sizeof (float) * obj.vertices.count));
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)(sizeof (float) * (obj.vertices.count + obj.uv.count)));
	glEnableVertexAttribArray (2);

	glBindVertexArray (0);

	return result;
}

void model_set_parent (model* main, model* parent) {
	main -> parent = parent;
	recalculate_world_position (main);
}

void model_set_position (model* m, glm::vec3 position) {
	m -> position = position;
	recalculate_world_position (m);
}

void model_render (model* m) {
	if (!m -> visible)
		return;

	shader_use (m -> shader_id);

	glBindVertexArray (m -> VAO);

	glm::vec3 final_position = m -> position;
	model* root = m -> parent;
	while (root) {
		final_position += root -> position;
		root = root -> parent;
	}

	glm::mat4 model = glm::mat4 (1.0f);
	model = glm::translate (model, final_position);
	model = glm::scale (model, glm::vec3 (m -> scale, m -> scale, m -> scale));
	shader_set_mat4 (m -> shader_id, "model", model);

	for (unsigned i = 0; i < m -> obj.vertex_groups.count; ++i) {
		unsigned material_id = m -> obj.vertex_groups.data[i].material_id;
		MTL_mat mat = m -> obj.material_library.materials[material_id];
		shader_set_vec3 (m -> shader_id, "ambient_color", glm::vec3 (mat.ambient_color[0], mat.ambient_color[1], mat.ambient_color[2]));
		shader_set_vec3 (m -> shader_id, "diffuse_color", glm::vec3 (mat.diffuse_color[0], mat.diffuse_color[1], mat.diffuse_color[2]));
		shader_set_vec3 (m -> shader_id, "specular_color", glm::vec3 (mat.specular_color[0], mat.specular_color[1], mat.specular_color[2]));
		shader_set_vec3 (m -> shader_id, "multiply_color", glm::vec3 (m -> multiply_color.x, m -> multiply_color.y, m -> multiply_color.z));
 		shader_set_float (m -> shader_id, "specular_exponent", mat.specular_exponent);
		glDrawArrays (GL_TRIANGLES, m -> obj.vertex_groups.data[i].start_index, 
									(m -> obj.vertex_groups.data[i].end_index - m -> obj.vertex_groups.data[i].start_index) + 1);
	}
	
	glBindVertexArray (0);
}
