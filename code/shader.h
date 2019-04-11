#if !defined (SHADER_H)
#define SHADER_H

#include "glm/glm.hpp"

unsigned shader_new (const char* vert_source, const char* farg_source);

void shader_use (unsigned shader);

void shader_set_vec3 (unsigned shader, const char* name, glm::vec3 value);
void shader_set_mat4 (unsigned shader, const char* name, glm::mat4 matrix);

#endif
