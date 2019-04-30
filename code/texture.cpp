#include "texture.h"

#include "gl_extensions.h"
#include "gl/gl.h"

texture* texture_new (unsigned width, unsigned height, unsigned char* data) {
	texture* result = (texture*)malloc (sizeof (texture));
	result -> size = glm::vec2 (width, height);

	glGenTextures (1, &result -> id);

	glBindTexture (GL_TEXTURE_2D, result -> id);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture (GL_TEXTURE_2D, 0);

	return result;
}
