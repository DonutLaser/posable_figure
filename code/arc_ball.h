#if !defined (ARC_BALL_H)
#define ARC_BALL_H

#include "camera.h"

#define ARC_BALL_SPEED 45.0f
#define ARC_BALL_SENSITIVITY 0.009f;
#define MIN_RADIUS 5.0f
#define MAX_RADIUS 15.0f

struct glm:: vec3;

struct arc_ball {
	camera cam;
	glm::vec3 target;
	float radius;
	float yaw;
	float pitch;
};

arc_ball arc_ball_new (glm::vec3 target);

void arc_ball_rotate (arc_ball* ball);
void arc_ball_zoom (arc_ball* ball, int zoom_dir);

#endif