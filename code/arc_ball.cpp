#include "arc_ball.h"

#include "glm/glm.hpp"

#define DEF_RADIUS 10.0f
#define MIN_RADIUS 5.0f
#define MAX_RADIUS 15.0f
#define ZOOM_SPEED 10.0f

arc_ball arc_ball_new (glm::vec3 target) {
	arc_ball result = { };
	result.radius = DEF_RADIUS;
	result.cam = camera_new (glm::vec3 (0.0f, 0.0f, result.radius));
	result.target = target;

	result.cam.front = glm::normalize (result.target - result.cam.position);
 	result.cam.right = glm::normalize (glm::cross (result.cam.front, result.cam.world_up));
 	result.cam.up = glm::normalize (glm::cross (result.cam.right, result.cam.front));

	return result;
}

void arc_ball_rotate (arc_ball* ball) {
	ball -> pitch = glm::clamp (ball -> pitch, -89.0f, 89.0f);

	glm::vec3 new_position;
	new_position.x = glm::sin (glm::radians (ball -> yaw)) * glm::cos (glm::radians (ball -> pitch)) * ball -> radius;
	new_position.y = glm::sin (glm::radians (ball -> pitch)) * ball -> radius;
	new_position.z = glm::cos (glm::radians (ball -> yaw)) * glm::cos (glm::radians (ball -> pitch)) * ball -> radius;

	ball -> cam.front = glm::normalize (ball -> target - new_position);
 	ball -> cam.right = glm::normalize (glm::cross (ball -> cam.front, ball -> cam.world_up));
 	ball -> cam.up = glm::normalize (glm::cross (ball -> cam.right, ball -> cam.front));

 	ball -> cam.position = new_position;
}

void arc_ball_zoom (arc_ball* ball, int zoom_dir) {
	ball -> radius += zoom_dir;
	ball -> radius  = glm::clamp (ball -> radius, MIN_RADIUS, MAX_RADIUS);

	arc_ball_rotate (ball);
}

