#if !defined (CAMERA_H)
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define DEF_YAW	-90.0f
#define DEF_PITCH 0.0f
#define DEF_ZOOM 45.0f

struct camera {
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;

	float yaw;
	float pitch;
	float zoom;
};

camera camera_new (glm::vec3 position, glm::vec3 up = glm::vec3 (0.0f, 1.0f, 0.0f));

glm::mat4 camera_get_view_matrix (camera* cam);

#endif