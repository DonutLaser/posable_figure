#if !defined (POSABLE_FIGURE_H)
#define POSABLE_FIGURE_H

#include "model.h"
#include "arc_ball.h"

struct input;
struct platform_api;

enum figrure_part { 
	FP_PELVIS, 
	FP_CHEST, 
	FP_HEAD, 
	FP_L_UPPER_ARM, FP_L_LOWER_ARM, FP_L_HAND,
	FP_R_UPPER_ARM, FP_R_LOWER_ARM, FP_R_HAND,
	FP_L_UPPER_LEG, FP_L_LOWER_LEG, FP_L_FOOT,
	FP_R_UPPER_LEG, FP_R_LOWER_LEG, FP_R_FOOT,
	FP_COUNT
};

struct app {
	model* figure[FP_COUNT];
	model* gizmo;
	arc_ball camera;

	bool orthographic_camera;
};
 
void app_init (void* memory, platform_api platform);
void app_update_and_render (void* memory, platform_api platform, input in, float dt);
void app_close (void* memory);

#endif