#if !defined (TEXTURE_H)
#define TEXTURE_H

#include "glm/glm.hpp"

struct texture {
	unsigned id;
	glm::vec2 size;
};

texture* texture_new (unsigned width, unsigned height, unsigned char* data);

#endif