#include "shader.h"

#include <stdio.h>

#include "gl_extensions.h"
#include <gl/gl.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

static void compile_shader (unsigned shader, const char* type) {
	glCompileShader (shader);

	int success;
	char info_log[512];
	glGetShaderiv (shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog (shader, 512, NULL, info_log);
		printf ("Unable to compile %s shader: %s\n", type, info_log);
	}
}

unsigned shader_new (const char* vert_source, const char* frag_source) {
	unsigned vert_shader = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vert_shader, 1, &vert_source, NULL);
	compile_shader (vert_shader, "vertex");

	unsigned frag_shader = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (frag_shader, 1, &frag_source, NULL);
	compile_shader (frag_shader, "fragment");

	unsigned full_shader = glCreateProgram ();
	glAttachShader (full_shader, vert_shader);
	glAttachShader (full_shader, frag_shader);
	glLinkProgram (full_shader);

	int success;
	char info_log[512];
	glGetProgramiv (full_shader, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog (full_shader, 512, NULL, info_log);
		printf ("Unablle to link the full shader: %s\n", info_log);
	}

	glDeleteShader (vert_shader);
	glDeleteShader (frag_shader);

	return full_shader;
}

void shader_use (unsigned shader) {
	glUseProgram (shader);
}

void shader_set_float (unsigned shader, const char* name, float value) {
	unsigned location = glGetUniformLocation (shader, name);
	glUniform1f (location, value);
}

void shader_set_vec3 (unsigned shader, const char* name, glm::vec3 value) {
	unsigned location = glGetUniformLocation (shader, name);
	glUniform3f (location, value.x, value.y, value.z);
}

void shader_set_mat4 (unsigned shader, const char* name, glm::mat4 matrix) {
	unsigned location = glGetUniformLocation (shader, name);
	glUniformMatrix4fv (location, 1, GL_FALSE, glm::value_ptr (matrix));
}