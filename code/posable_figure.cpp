#include "posable_figure.h"

#include "glm/glm.hpp"

#include "obj_loader.h"
#include "platform.h"
#include "shader.h"
#include "ui.h"
#include "constants.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gl_extensions.h"
#include <gl/gl.h>

static void update_projection (app* App, unsigned window_width, unsigned window_height) {
	float zoom_factor = App -> camera.radius / MAX_RADIUS;
	camera_recalculate_projection (&App -> camera.cam, (float)window_width, (float)window_height, zoom_factor);

	shader_use (App -> figure[0] -> m -> shader_id);
	for (unsigned i = 0; i < FP_COUNT; ++i) 
		shader_set_mat4 (App -> figure[i] -> m -> shader_id, "projection", App -> camera.cam.projection);

	shader_use (App -> rotation_gizmo[0] -> m -> shader_id);
	for (unsigned i = 0; i < GP_COUNT; ++i)
		shader_set_mat4 (App -> rotation_gizmo[i] -> m -> shader_id, "projection", App -> camera.cam.projection);

	glm::mat4 ui_projection = glm::ortho (0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
	shader_use (App -> UI -> shader_id);
	shader_set_mat4 (App -> UI -> shader_id, "projection", ui_projection);
}

static void reset_pose (app* App) {
	App -> rotation_axis = GP_COUNT;

	if (App -> selected_figure_part) {
		App -> selected_figure_part -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);
		App -> selected_figure_part = NULL;
	}

	for (unsigned i = 0; i < FP_COUNT; ++i)
		transform_set_rotation (App -> figure[i] -> t, glm::quat (glm::vec3 (0.0f, 0.0f, 0.0f)));
}

static void handle_input (app* App, platform_api api, input in, float dt) {
	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);

	// Do the rotation
	if (App -> rotation_axis != GP_COUNT) {
		int x_delta = in.mouse_x - in.last_mouse_x;
		if (in.lmb_down) {
			if (App -> rotation_axis == GP_COUNT) {
				App -> selected_figure_part -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);
				App -> selected_figure_part = NULL;
			}
			else {
				App -> rotation_axis = GP_COUNT;
				in.lmb_down = false;

				for (unsigned i = 0; i < GP_COUNT; ++i)
					App -> rotation_gizmo[i] -> m -> visible = true;
			}
		}
		else if (in.rmb_down) {
			transform_set_rotation (App -> selected_figure_part -> t, App -> last_rotation);
			App -> rotation_axis = GP_COUNT;

			glm::vec3 world_position = transform_get_world_position (App -> selected_figure_part -> t);
			glm::quat world_rotation = transform_get_world_rotation (App -> selected_figure_part -> t);

			for (unsigned i = 0; i < GP_COUNT; ++i) {
				transform_set_position (App -> rotation_gizmo[i] -> t, world_position);
				transform_set_rotation (App -> rotation_gizmo[i] -> t, world_rotation);
			}
		}
		else {
			if (App -> rotation_axis == GP_X_AXIS) {	
				transform_rotate (App -> selected_figure_part -> t, App -> selected_figure_part -> t -> right, 
								 x_delta * ROTATION_SPEED);

				for (unsigned i = 0; i < GP_COUNT; ++i) {
					transform_rotate (App -> rotation_gizmo[i] -> t, App -> selected_figure_part -> t -> right,
									  x_delta * ROTATION_SPEED);
				}
			}
			else if (App -> rotation_axis == GP_Y_AXIS) {
				transform_rotate (App -> selected_figure_part -> t, App -> selected_figure_part -> t -> up,
								  x_delta * ROTATION_SPEED);

				for (unsigned i = 0; i < GP_COUNT; ++i) {
					transform_rotate (App -> rotation_gizmo[i] -> t, App -> selected_figure_part -> t -> up,
									  x_delta * ROTATION_SPEED);
				}
			}
			else if (App -> rotation_axis == GP_Z_AXIS) {
				transform_rotate (App -> selected_figure_part -> t, App -> selected_figure_part -> t -> forward,
								  x_delta * ROTATION_SPEED);

				for (unsigned i = 0; i < GP_COUNT; ++i) {
					transform_rotate (App -> rotation_gizmo[i] -> t, App -> selected_figure_part -> t -> forward,
									  x_delta * ROTATION_SPEED);
				}
			}
		}
	}

	unsigned index = 0;
	glReadPixels (in.mouse_x, window_height - in.mouse_y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

	// Do the rotation axis selection
	if (App -> selected_figure_part) {
		bool axis_selected = false;
		if (in.x_down || in.y_down || in.z_down) {
			if (in.x_down)
				App -> rotation_axis = GP_X_AXIS;
			else if (in.y_down)
				App -> rotation_axis = GP_Y_AXIS;
			else if (in.z_down)
				App -> rotation_axis = GP_Z_AXIS;
		}
		else if (index != 0 && index >= 100) {
			App -> rotation_gizmo[index - 100] -> m -> multiply_color = glm::vec3 (SELECTION_MULTIPLIER_COLOR);

			if (in.lmb_down) {
				App -> rotation_axis = (gizmo_part)(index - 100);
				axis_selected = true;

				for (unsigned i = 0; i < GP_COUNT; ++i)
					App -> rotation_gizmo[i] -> m -> visible = false;
			}
		}
		else {
			for (unsigned i = 0; i < GP_COUNT; ++i)
				App -> rotation_gizmo[i] -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);
		}

		if (in.lmb_down && !axis_selected) {
			if (App -> rotation_axis == GP_COUNT) {
				App -> selected_figure_part -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);
				App -> selected_figure_part = NULL;
			}
			else
				App -> rotation_axis = GP_COUNT;
		}
	}

	// Do the figure part selection
	if (App -> rotation_axis == GP_COUNT) {
		if (index != 0 && index <= FP_COUNT) {
			if (App -> hover_figure_part && App -> hover_figure_part != App -> selected_figure_part)
				App -> hover_figure_part -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);

			App -> hover_figure_part = App -> figure[index - 1];
			App -> hover_figure_part -> m -> multiply_color = glm::vec3 (SELECTION_MULTIPLIER_COLOR);

			if (in.lmb_down) {
				if (App -> selected_figure_part)
					App -> selected_figure_part -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);

				App -> selected_figure_part = App -> figure[index - 1];
				App -> selected_figure_part -> m -> multiply_color = glm::vec3 (SELECTION_MULTIPLIER_COLOR);

				App -> hover_figure_part = NULL;

				glm::vec3 world_position = transform_get_world_position (App -> selected_figure_part -> t);
				glm::quat world_rotation = transform_get_world_rotation (App -> selected_figure_part -> t);
				App -> last_rotation = App -> selected_figure_part -> t -> rotation;

				for (unsigned i = 0; i < GP_COUNT; ++i) {
					transform_set_position (App -> rotation_gizmo[i] -> t, world_position);
					transform_set_rotation (App -> rotation_gizmo[i] -> t, world_rotation);
				}
			}
		}
		else {
			if (App -> hover_figure_part && App -> hover_figure_part != App -> selected_figure_part)
				App -> hover_figure_part -> m -> multiply_color = glm::vec3 (1.0f, 1.0f, 1.0f);
		}
	}
	

	if (in.rmb_pressed) {
		int mouse_delta_x = in.last_mouse_x - in.mouse_x;
		int mouse_delta_y = in.mouse_y - in.last_mouse_y;

		App -> camera.yaw += mouse_delta_x * ARC_BALL_SPEED * ARC_BALL_SENSITIVITY;
		App -> camera.pitch += mouse_delta_y * ARC_BALL_SPEED * ARC_BALL_SENSITIVITY;

		arc_ball_rotate (&App -> camera);
	}
	else if (in.num1_down) {
		App -> camera.yaw = FRONT_VIEW_YAW;
		App -> camera.pitch = FRONT_VIEW_PITCH;

		arc_ball_rotate (&App -> camera);
	}
	else if (in.num3_down) {
		App -> camera.yaw = RIGHT_VIEW_YAW;
		App -> camera.pitch = RIGHT_VIEW_PITCH;

		arc_ball_rotate (&App -> camera);
	}
	else if (in.num7_down) {
		App -> camera.yaw = TOP_VIEW_YAW;
		App -> camera.pitch = TOP_VIEW_PITCH;

		arc_ball_rotate (&App -> camera);
	}
	else if (in.num5_down) {
		App -> camera.cam.orthographic = !App -> camera.cam.orthographic;

		update_projection (App, window_width, window_height);
	}

	if (in.scroll_dir != 0) {
		arc_ball_zoom (&App -> camera, -in.scroll_dir);
		if (App -> camera.cam.orthographic)
			update_projection (App, window_width, window_height);
	}
}

static unsigned load_shader (platform_api api, const char* vert_path, const char* frag_path) {
	char* vert_source; 
	char* frag_source;
	api.read_file (vert_path, &vert_source);
	api.read_file (frag_path, &frag_source);

	unsigned result = shader_new (vert_source, frag_source);

	free (vert_source);
	free (frag_source);

	return result;
}

static unsigned char* load_texture (const char* path, unsigned* width, unsigned* height) {
	int channels = 0, result_width, result_height;
	unsigned char* result = stbi_load (path, &result_width, &result_height, &channels, 0);

	*width = (unsigned)result_width;
	*height = (unsigned)result_height;

	return result;
}

static void setup_gizmo (app* App, unsigned shader) {
	model* x_axis = model_new (obj_load (GIZMO_X), shader);
	model* y_axis = model_new (obj_load (GIZMO_Y), shader);
	model* z_axis = model_new (obj_load (GIZMO_Z), shader);

	App -> rotation_gizmo[GP_X_AXIS] = object_new (x_axis);
	App -> rotation_gizmo[GP_Y_AXIS] = object_new (y_axis);
	App -> rotation_gizmo[GP_Z_AXIS] = object_new (z_axis);

	for (unsigned i = 0; i < GP_COUNT; ++i)
		transform_set_scale (App -> rotation_gizmo[i] -> t, glm::vec3 (0.25f, 0.25f, 0.25f));
}

static void setup_figure (app* App, unsigned shader) {
	model* head_model = model_new (obj_load (HEAD), shader);
	model* chest_model = model_new (obj_load (CHEST), shader);
	model* pelvis_model = model_new (obj_load (PELVIS), shader);
	model* l_upper_arm_model = model_new (obj_load (LEFT_UPPER_ARM), shader);
	model* l_lower_arm_model = model_new (obj_load (LEFT_LOWER_ARM), shader);
	model* l_hand_model = model_new (obj_load (LEFT_HAND), shader);
	model* r_upper_arm_model = model_new (obj_load (RIGHT_UPPER_ARM), shader);
	model* r_lower_arm_model = model_new (obj_load (RIGHT_LOWER_ARM), shader);
	model* r_hand_model = model_new (obj_load (RIGHT_HAND), shader);
	model* l_upper_leg_model = model_new (obj_load (LEFT_UPPER_LEG), shader);
	model* l_lower_leg_model = model_new (obj_load (LEFT_LOWER_LEG), shader);
	model* l_foot_model = model_new (obj_load (LEFT_FOOT), shader);
	model* r_upper_leg_model = model_new (obj_load (RIGHT_UPPER_LEG), shader);
	model* r_lower_leg_model = model_new (obj_load (RIGHT_LOWER_LEG), shader);
	model* r_foot_model = model_new (obj_load (RIGHT_FOOT), shader);

	App -> figure[FP_PELVIS] = object_new (pelvis_model);
	App -> figure[FP_CHEST] = object_new (chest_model);
	transform_set_parent (App -> figure[FP_CHEST] -> t, App -> figure[FP_PELVIS] -> t);
	App -> figure[FP_HEAD] = object_new (head_model);
	transform_set_parent (App -> figure[FP_HEAD] -> t, App -> figure[FP_CHEST] -> t);
	App -> figure[FP_L_UPPER_ARM] = object_new (l_upper_arm_model);
	transform_set_parent (App -> figure[FP_L_UPPER_ARM] -> t, App -> figure[FP_CHEST] -> t);
	App -> figure[FP_L_LOWER_ARM] = object_new (l_lower_arm_model);
	transform_set_parent (App -> figure[FP_L_LOWER_ARM] -> t, App -> figure[FP_L_UPPER_ARM] -> t);
	App -> figure[FP_L_HAND] = object_new (l_hand_model);
	transform_set_parent (App -> figure[FP_L_HAND] -> t, App -> figure[FP_L_LOWER_ARM] -> t);
	App -> figure[FP_R_UPPER_ARM] = object_new (r_upper_arm_model);
	transform_set_parent (App -> figure[FP_R_UPPER_ARM] -> t, App -> figure[FP_CHEST] -> t); 
	App -> figure[FP_R_LOWER_ARM] = object_new (r_lower_arm_model);
	transform_set_parent (App -> figure[FP_R_LOWER_ARM] -> t, App -> figure[FP_R_UPPER_ARM] -> t);
	App -> figure[FP_R_HAND] = object_new (r_hand_model);
	transform_set_parent (App -> figure[FP_R_HAND] -> t, App -> figure[FP_R_LOWER_ARM] -> t);
	App -> figure[FP_L_UPPER_LEG] = object_new (l_upper_leg_model);
	transform_set_parent (App -> figure[FP_L_UPPER_LEG] -> t, App -> figure[FP_PELVIS] -> t);
	App -> figure[FP_L_LOWER_LEG] = object_new (l_lower_leg_model);
	transform_set_parent (App -> figure[FP_L_LOWER_LEG] -> t, App -> figure[FP_L_UPPER_LEG] -> t);
	App -> figure[FP_L_FOOT] = object_new (l_foot_model);
	transform_set_parent (App -> figure[FP_L_FOOT] -> t, App -> figure[FP_L_LOWER_LEG] -> t);
	App -> figure[FP_R_UPPER_LEG] = object_new (r_upper_leg_model);
	transform_set_parent (App -> figure[FP_R_UPPER_LEG] -> t, App -> figure[FP_PELVIS] -> t);
	App -> figure[FP_R_LOWER_LEG] = object_new (r_lower_leg_model);
	transform_set_parent (App -> figure[FP_R_LOWER_LEG] -> t, App -> figure[FP_R_UPPER_LEG] -> t);
	App -> figure[FP_R_FOOT] = object_new (r_foot_model);
	transform_set_parent (App -> figure[FP_R_FOOT] -> t, App -> figure[FP_R_LOWER_LEG] -> t);

	transform_set_position (App -> figure[FP_PELVIS] -> t, glm::vec3 (0.0f, 0.0f, 0.0f));
	transform_set_position (App -> figure[FP_CHEST] -> t, glm::vec3 (0.0f, 0.5f, 0.0f));
	transform_set_position (App -> figure[FP_HEAD] -> t, glm::vec3 (0.0f, 0.9f, 0.0f));
	transform_set_position (App -> figure[FP_L_UPPER_ARM] -> t, glm::vec3 (0.32f, 0.69f, -0.05f));
	transform_set_position (App -> figure[FP_L_LOWER_ARM] -> t, glm::vec3 (0.625f, 0.012f, 0.0f));
	transform_set_position (App -> figure[FP_L_HAND] -> t, glm::vec3 (0.68f, -0.028f, 0.0f));
	transform_set_position (App -> figure[FP_R_UPPER_ARM] -> t, glm::vec3 (-0.32f, 0.69f, -0.05f));
	transform_set_position (App -> figure[FP_R_LOWER_ARM] -> t, glm::vec3 (-0.625f, 0.012f, 0.0f));
	transform_set_position (App -> figure[FP_R_HAND] -> t, glm::vec3 (-0.68f, -0.028f, 0.0f));
	transform_set_position (App -> figure[FP_L_UPPER_LEG] -> t, glm::vec3 (0.15f, -0.115f, -0.005f));
	transform_set_position (App -> figure[FP_L_LOWER_LEG] -> t, glm::vec3 (-0.02f, -1.01f, 0.0f));
	transform_set_position (App -> figure[FP_L_FOOT] -> t, glm::vec3 (0.0f, -1.05f, 0.0f));
	transform_set_position (App -> figure[FP_R_UPPER_LEG] -> t, glm::vec3 (-0.15f, -0.115f, -0.005f));
	transform_set_position (App -> figure[FP_R_LOWER_LEG] -> t, glm::vec3 (0.02f, -1.01f, 0.0f));
	transform_set_position (App -> figure[FP_R_FOOT] -> t, glm::vec3 (0.0f, -1.05f, 0.0f));
}

static void render_gizmo (app* App, glm::mat4 view_matrix) {
	if (!App -> selected_figure_part)
		return;

	unsigned shader = App -> rotation_gizmo[0] -> m -> shader_id;
	shader_use (shader);
	shader_set_mat4 (shader, "view", view_matrix);
	for (unsigned i = 0; i < GP_COUNT; ++i) {
		glStencilFunc (GL_ALWAYS, i + 100, 0xFF);
		object_render (App -> rotation_gizmo[i]);
	}
}

static void render_figure (app* App, glm::mat4 view_matrix) {
	unsigned shader = App -> figure[0] -> m -> shader_id;
	shader_use (shader);
	shader_set_mat4 (shader, "view", view_matrix);
	shader_set_vec3 (shader, "view_direction", App -> camera.cam.front);
	for (unsigned i = 0; i < FP_COUNT; ++i) {
		glStencilFunc (GL_ALWAYS, i + 1, 0xFF);
		object_render (App -> figure[i]);
	}
}

void app_init (void* memory, platform_api api) {
	app* App = (app*)memory;

	unsigned default_shader = load_shader (api, DEFAULT_VERT_SOURCE, DEFAULT_FRAG_SOURCE);
	unsigned unlit_shader = load_shader (api, UNLIT_VERT_SOURCE, UNLIT_FRAG_SOURCE);
	unsigned ui_shader = load_shader (api, UI_VERT_SOURCE, UI_FRAG_SOURCE);

	setup_figure (App, default_shader);
	setup_gizmo (App, unlit_shader);
	App -> UI = ui_new (ui_shader);

	App -> camera = arc_ball_new (glm::vec3 (0.0f, 0.0f, 0.0f));
	App -> camera.cam.orthographic = false;
	App -> camera.cam.near_plane = 0.1f;
	App -> camera.cam.far_plane = 100.0f;

	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);
	update_projection (App, window_width, window_height);

	glm::vec2 button_size = glm::vec2 (RESET_BUTTON_SIZE);
	glm::vec2 button_position = glm::vec2 (window_width - 10 - button_size.x, window_height - 10 - button_size.y);
	App -> button = ui_button_new (button_position, button_size);

	shader_use (App -> figure[0] -> m -> shader_id);
	shader_set_vec3 (App -> figure[0] -> m -> shader_id, "light_direction", glm::vec3 (-1.0f, -1.0f, 0.0f));

	glEnable (GL_DEPTH_TEST);
	glEnable (GL_STENCIL_TEST);
	glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask (0xFF);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	App -> rotation_axis = GP_COUNT;
	App -> selected_figure_part = NULL;
	App -> hover_figure_part = NULL;
	App -> last_rotation = glm::quat (glm::vec3 (0.0f, 0.0f, 0.0f));

	const char* paths[TT_COUNT] = {
		RESET_IMAGE, ORTHO_IMAGE, PERSP_IMAGE,
		DEFAULT_IMAGE
	};

	for (unsigned i = 0; i < TT_COUNT; ++i) {
		unsigned width, height;
		unsigned char* data = load_texture (paths[i], &width, &height);
		App -> textures[i] = texture_new (width, height, data);
	}

	App -> button -> tex = App -> textures[TT_RESET];
	App -> UI -> default_texture = App -> textures[TT_DEFAULT];
}

void app_update_and_render (void* memory, platform_api api, input in, float dt) {
	app* App = (app*)memory;

	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);

	if (api.was_window_resized ()) {
		update_projection (App, window_width, window_height);
		App -> button -> position = glm::vec2 (window_width - 10 - App -> button -> size.x,
											   window_height - 10 - App -> button -> size.y);
	}

	handle_input (App, api, in, dt);

	glClearColor (BG_COLOR);
	glClearStencil (0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glDisable (GL_DEPTH_TEST);
	glDisable (GL_STENCIL_TEST);
	if (ui_do_button (App -> UI, App -> button, in, glm::vec3 (RESET_BUTTON_COLOR), glm::vec3 (RESET_BUTTON_COLOR_HOVER), glm::vec3 (RESET_BUTTON_COLOR_CLICK)))
		reset_pose (App);

	ui_render_texture (App -> UI, glm::vec2 (10.0f, 10.0f), App -> camera.cam.orthographic ? App -> textures[TT_ORTHO] : App -> textures[TT_PERSP]);

	glEnable (GL_DEPTH_TEST);
	glEnable (GL_STENCIL_TEST);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);

	render_figure (App, view);

	glDisable (GL_DEPTH_TEST);
	render_gizmo (App, view);
	glEnable (GL_DEPTH_TEST);
}

void app_close (void* memory) {
	app* App = (app*)memory;

	for (unsigned i = 0; i < FP_COUNT; ++i) {
		object_free (App -> figure[i]);
		free (App -> figure[i]);
	}

	for (unsigned i = 0; i < GP_COUNT; ++i) {
		object_free (App -> rotation_gizmo[i]);
		free (App -> rotation_gizmo[i]);
	}

	free (App -> UI);
	free (App -> button);
}
