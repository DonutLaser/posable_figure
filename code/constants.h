#if !defined (CONSTANTS_H)
#define CONSTANTS_H

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BG_COLOR 0.239f, 0.227f, 0.419f, 1.0f
#define SELECTION_MULTIPLIER_COLOR 0.5f, 0.5f, 0.5f

#define DEFAULT_VERT_SOURCE		"../data/shaders/default.vert"
#define DEFAULT_FRAG_SOURCE		"../data/shaders/default.frag"
#define UNLIT_VERT_SOURCE		"../data/shaders/unlit.vert"
#define UNLIT_FRAG_SOURCE		"../data/shaders/unlit.frag"
#define UI_VERT_SOURCE 			"../data/shaders/ui.vert"
#define UI_FRAG_SOURCE 			"../data/shaders/ui.frag"
#define UI_TEX_VERT_SOURCE		"../data/shaders/ui_texture.vert"
#define UI_TEX_FRAG_SOURCE		"../data/shaders/ui_texture.frag"

#define HEAD					"../data/models/head.obj"
#define CHEST					"../data/models/chest.obj"
#define PELVIS					"../data/models/pelvis.obj"
#define LEFT_UPPER_ARM			"../data/models/left_upper_arm.obj"
#define LEFT_LOWER_ARM			"../data/models/left_lower_arm.obj"
#define LEFT_HAND				"../data/models/left_hand.obj"
#define RIGHT_UPPER_ARM			"../data/models/right_upper_arm.obj"
#define RIGHT_LOWER_ARM			"../data/models/right_lower_arm.obj"
#define RIGHT_HAND				"../data/models/right_hand.obj"
#define LEFT_UPPER_LEG			"../data/models/left_upper_leg.obj"
#define LEFT_LOWER_LEG			"../data/models/left_lower_leg.obj"
#define LEFT_FOOT				"../data/models/left_foot.obj"
#define RIGHT_UPPER_LEG			"../data/models/right_upper_leg.obj"
#define RIGHT_LOWER_LEG			"../data/models/right_lower_leg.obj"
#define RIGHT_FOOT				"../data/models/right_foot.obj"

#define GIZMO_X					"../data/models/gizmo_x.obj"
#define GIZMO_Y					"../data/models/gizmo_y.obj"
#define GIZMO_Z					"../data/models/gizmo_z.obj"

#define RESET_IMAGE				"../data/images/reset.png"
#define ORTHO_IMAGE				"../data/images/orthographic.png"
#define PERSP_IMAGE				"../data/images/perspective.png"
#define ICON_IMAGE				"../data/images/icon.ico"

#define FRONT_VIEW_YAW 0.0f
#define FRONT_VIEW_PITCH 0.0f
#define RIGHT_VIEW_YAW 90.0f
#define RIGHT_VIEW_PITCH 0.0f
#define TOP_VIEW_YAW 0.0f
#define TOP_VIEW_PITCH 90.0f

#define ROTATION_SPEED 0.2f

#define RESET_BUTTON_SIZE 128.0f, 32.0f
#define RESET_BUTTON_COLOR 1.0f, 1.0f, 1.0f
#define RESET_BUTTON_COLOR_HOVER 0.847f, 0.847f, 0.847f
#define RESET_BUTTON_COLOR_CLICK 0.737f, 0.737f, 0.737f

#endif