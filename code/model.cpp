#include "model.h"
#include "shader.h"

#include "gl_extensions.h"
#include <gl/gl.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

model model_new (float* vertices, unsigned vert_count, float* uvs, unsigned uv_count, float* normals, unsigned normal_count, material mat) {
	model result = { };
	result.vertex_count = vert_count / 3;
	result.position = glm::vec3 (0.0f, 0.0f, 0.0f);
	result.mat = mat;

	glGenVertexArrays (1, &result.VAO);
	glGenBuffers (1, &result.VBO);

	glBindVertexArray (result.VAO);

	glBindBuffer (GL_ARRAY_BUFFER, result.VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (float) * (vert_count + uv_count + normal_count), NULL, GL_STATIC_DRAW);

	glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof (float) * vert_count, vertices);
	glBufferSubData (GL_ARRAY_BUFFER, sizeof (float) * vert_count, sizeof (float) * uv_count, uvs);
	glBufferSubData (GL_ARRAY_BUFFER, sizeof (float) * (vert_count + uv_count), sizeof (float) * normal_count, normals);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (void*)(sizeof (float) * vert_count));
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)(sizeof (float) * (vert_count + uv_count)));
	glEnableVertexAttribArray (2);

	glBindVertexArray (0);

	return result;
}

void model_render (model* m) {
	shader_use (m -> mat.shader);

	glm::mat4 model = glm::mat4 (1.0f);
	model = glm::translate (model, m -> position);
	shader_set_mat4 (m -> mat.shader, "model", model);

	glBindVertexArray (m -> VAO);
	glDrawArrays (GL_TRIANGLES, 0, m -> vertex_count);
	
	glBindVertexArray (0);
}