#if !defined (MATERIAL_H)
#define MATERIAL_H

#include "glm/glm.hpp"

struct material {
	glm::vec3 diffuse_color;
	// glm::vec3 ambient_color;

	// unsigned main_texture_id
	unsigned shader;
};

material material_new (unsigned shader);

#endif