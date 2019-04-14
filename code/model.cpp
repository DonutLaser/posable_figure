#include "model.h"
#include "shader.h"

#include "gl_extensions.h"
#include <gl/gl.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

model model_new (OBJ obj, material mat) {
	model result = { };
	result.obj = obj;
	result.vertex_count = obj.vertices.count / 3;
	result.position = glm::vec3 (0.0f, 0.0f, 0.0f);
	result.mat = mat;

	glGenVertexArrays (1, &result.VAO);
	glGenBuffers (1, &result.VBO);

	glBindVertexArray (result.VAO);

	glBindBuffer (GL_ARRAY_BUFFER, result.VBO);
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

void model_render (model* m) {
	shader_use (m -> mat.shader);

	glBindVertexArray (m -> VAO);

	glm::mat4 model = glm::mat4 (1.0f);
	model = glm::translate (model, m -> position);
	shader_set_mat4 (m -> mat.shader, "model", model);

	for (unsigned i = 0; i < m -> obj.vertex_groups.count; ++i) {
		unsigned material_id = m -> obj.vertex_groups.data[i].material_id;
		MTL_mat mat = m -> obj.material_library.materials[material_id];
		shader_set_vec3 (m -> mat.shader, "ambient_color", glm::vec3 (mat.ambient_color[0], mat.ambient_color[1], mat.ambient_color[2]));
		shader_set_vec3 (m -> mat.shader, "diffuse_color", glm::vec3 (mat.diffuse_color[0], mat.diffuse_color[1], mat.diffuse_color[2]));
		shader_set_vec3 (m -> mat.shader, "specular_color", glm::vec3 (mat.specular_color[0], mat.specular_color[1], mat.specular_color[2]));
 		shader_set_float (m -> mat.shader, "specular_exponent", mat.specular_exponent);
		glDrawArrays (GL_TRIANGLES, m -> obj.vertex_groups.data[i].start_index, 
									(m -> obj.vertex_groups.data[i].end_index - m -> obj.vertex_groups.data[i].start_index) + 1);
	}
	
	glBindVertexArray (0);
}