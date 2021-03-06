#if !defined (POSABLE_FIGURE_H)
#define POSABLE_FIGURE_H

#include "object.h"
#include "arc_ball.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"

struct input;
struct platform_api;
struct ui;
struct ui_button;
struct texture;

enum figure_part { 
	FP_PELVIS,
	FP_CHEST, 
	FP_HEAD, 
	FP_L_UPPER_ARM, FP_L_LOWER_ARM, FP_L_HAND,
	FP_R_UPPER_ARM, FP_R_LOWER_ARM, FP_R_HAND,
	FP_L_UPPER_LEG, FP_L_LOWER_LEG, FP_L_FOOT,
	FP_R_UPPER_LEG, FP_R_LOWER_LEG, FP_R_FOOT,
	FP_COUNT
};

enum gizmo_part {
	GP_X_AXIS,
	GP_Y_AXIS,
	GP_Z_AXIS,
	GP_COUNT
};

enum texture_type { TT_RESET, TT_ORTHO, TT_PERSP, TT_COUNT };

struct app {
	object* figure[FP_COUNT];
	object* rotation_gizmo[GP_COUNT];
	arc_ball camera;

	ui* UI;
	ui_button* button;

	texture* textures[TT_COUNT];

	glm::vec3 rotation_axis;
	object* selected_figure_part;
	object* hover_figure_part;
	glm::quat last_rotation;
};
 
void app_init (void* memory, platform_api platform);
void app_update_and_render (void* memory, platform_api platform, input in, float dt);
void app_close (void* memory);

#endif