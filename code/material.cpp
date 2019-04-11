#include "material.h"

#include "shader.h"

material material_new (unsigned shader) {
	material result = { };
	result.shader = shader;

	return result;
}