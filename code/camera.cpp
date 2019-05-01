#include "camera.h"
#include <math.h>

#define DEFAULT_NEAR	0.1f
#define DEFAULT_FAR		100.0f

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
	result.near_plane = DEFAULT_NEAR;
	result.far_plane = DEFAULT_FAR;

	result.position = position;
	result.world_up = up;

	update_camera_vectors (&result);

	return result;
}

glm::mat4 camera_get_view_matrix (camera* cam) {
	return glm::lookAt (cam -> position, cam -> position + cam -> front, cam -> up);
}

void camera_recalculate_projection (camera* cam, float window_width, float window_height, float orthographic_zoom_factor) {
	if (!cam -> orthographic) {
		if (window_width != 0 && window_height != 0)
			cam -> projection = glm::perspective (glm::radians (cam -> zoom), window_width / window_height, cam -> near_plane, cam -> far_plane);
	}
	else {
		float half_width = window_width / 100 / 2;
		float half_height = window_height / 100 / 2;
		cam -> projection = glm::ortho (-half_width * orthographic_zoom_factor,
										half_width * orthographic_zoom_factor,
										-half_height * orthographic_zoom_factor,
										half_height * orthographic_zoom_factor,
										-1.0f, cam -> far_plane);
	}
}
