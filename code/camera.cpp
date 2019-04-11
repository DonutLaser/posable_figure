#include "camera.h"
#include <math.h>

static void update_camera_vectors (camera* cam) {
 	glm::vec3 front_result;
 	front_result.x = cos (glm::radians (cam -> yaw)) * cos (glm::radians (cam -> pitch));
 	front_result.y = sin (glm::radians (cam -> pitch));
 	front_result.z = sin (glm::radians (cam -> yaw)) * cos (glm::radians (cam -> pitch));
 	cam -> front = glm::normalize (front_result);

 	cam -> right = glm::normalize (glm::cross (cam -> front, cam -> world_up));
 	cam -> up = glm::normalize (glm::cross (cam -> right, cam -> front));
}

camera camera_new (glm::vec3 position, glm::vec3 up) {
	camera result = { };
	result.front = glm::vec3 (0.0f, 0.0f, -1.0f);	
	result.yaw = DEF_YAW;
	result.pitch = DEF_PITCH;
	result.zoom = DEF_ZOOM;

	result.position = position;
	result.world_up = up;

	update_camera_vectors (&result);

	return result;
}

glm::mat4 camera_get_view_matrix (camera* cam) {
	return glm::lookAt (cam -> position, cam -> position + cam -> front, cam -> up);
}
