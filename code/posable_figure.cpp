#include "posable_figure.h"

#include "glm/glm.hpp"

#include "obj_loader.h"
#include "platform.h"
#include "shader.h"
#include "constants.h"

#include "gl_extensions.h"
#include <gl/gl.h>

static void update_projection (app* App, unsigned window_width, unsigned window_height) {
		glm::mat4 projection = glm::mat4 (1.0f);
		if (!App -> orthographic_camera)
			projection = glm::perspective (glm::radians (App -> camera.cam.zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
		else {
			float half_width = (float)window_width / 100 / 2;
			float half_height = (float)window_height / 100 / 2;
			float zoom_factor = App -> camera.radius / MAX_RADIUS;
			projection = glm::ortho (-half_width * zoom_factor, half_width * zoom_factor, -half_height * zoom_factor, half_height * zoom_factor, -1.0f, 100.0f);
		}

		shader_use (App -> figure[0] -> shader_id);
		for (unsigned i = 0; i < FP_COUNT; ++i) 
			shader_set_mat4 (App -> figure[i] -> shader_id, "projection", projection);

		shader_use (App -> gizmo -> shader_id);
		shader_set_mat4 (App -> gizmo -> shader_id, "projection", projection);
}

static void handle_input (app* App, platform_api api, input in, float dt) {
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
		App -> orthographic_camera = !App -> orthographic_camera;

		unsigned window_width, window_height;
		api.get_window_size (&window_width, &window_height);
		update_projection (App, window_width, window_height);
	}
	else if (in.f9_down)
		App -> debug = !App -> debug;

	if (in.scroll_dir != 0) {
		arc_ball_zoom (&App -> camera, -in.scroll_dir);
		if (App -> orthographic_camera) {
			unsigned window_width, window_height;
			api.get_window_size (&window_width, &window_height);
			update_projection (App, window_width, window_height);
		}
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

	App -> figure[FP_PELVIS] = pelvis_model;
	App -> figure[FP_CHEST] = chest_model;
	model_set_parent (App -> figure[FP_CHEST], App -> figure[FP_PELVIS]);
	App -> figure[FP_HEAD] = head_model;
	model_set_parent (App -> figure[FP_HEAD], App -> figure[FP_CHEST]);
	App -> figure[FP_L_UPPER_ARM] = l_upper_arm_model;
	model_set_parent (App -> figure[FP_L_UPPER_ARM], App -> figure[FP_CHEST]);
	App -> figure[FP_L_LOWER_ARM] = l_lower_arm_model;
	model_set_parent (App -> figure[FP_L_LOWER_ARM], App -> figure[FP_L_UPPER_ARM]);
	App -> figure[FP_L_HAND] = l_hand_model;
	model_set_parent (App -> figure[FP_L_HAND], App -> figure[FP_L_LOWER_ARM]);
	App -> figure[FP_R_UPPER_ARM] = r_upper_arm_model;
	model_set_parent (App -> figure[FP_R_UPPER_ARM], App -> figure[FP_CHEST]);
	App -> figure[FP_R_LOWER_ARM] = r_lower_arm_model;
	model_set_parent (App -> figure[FP_R_LOWER_ARM], App -> figure[FP_R_UPPER_ARM]);
	App -> figure[FP_R_HAND] = r_hand_model;
	model_set_parent (App -> figure[FP_R_HAND], App -> figure[FP_R_LOWER_ARM]);
	App -> figure[FP_L_UPPER_LEG] = l_upper_leg_model;
	model_set_parent (App -> figure[FP_L_UPPER_LEG], App -> figure[FP_PELVIS]);
	App -> figure[FP_L_LOWER_LEG] = l_lower_leg_model;
	model_set_parent (App -> figure[FP_L_LOWER_LEG], App -> figure[FP_L_UPPER_LEG]);
	App -> figure[FP_L_FOOT] = l_foot_model;
	model_set_parent (App -> figure[FP_L_FOOT], App -> figure[FP_L_LOWER_LEG]);
	App -> figure[FP_R_UPPER_LEG] = r_upper_leg_model;
	model_set_parent (App -> figure[FP_R_UPPER_LEG], App -> figure[FP_PELVIS]);
	App -> figure[FP_R_LOWER_LEG] = r_lower_leg_model;
	model_set_parent (App -> figure[FP_R_LOWER_LEG], App -> figure[FP_R_UPPER_LEG]);
	App -> figure[FP_R_FOOT] = r_foot_model;
	model_set_parent (App -> figure[FP_R_FOOT], App -> figure[FP_R_LOWER_LEG]);

	model_set_position (App -> figure[FP_PELVIS], glm::vec3 (0.0f, 0.0f, 0.0f));
	model_set_position (App -> figure[FP_CHEST], glm::vec3 (0.0f, 0.5f, 0.0f));
	model_set_position (App -> figure[FP_HEAD], glm::vec3 (0.0f, 0.9f, 0.0f));
	model_set_position (App -> figure[FP_L_UPPER_ARM], glm::vec3 (0.32f, 0.69f, -0.05f));
	model_set_position (App -> figure[FP_L_LOWER_ARM], glm::vec3 (0.625f, 0.012f, 0.0f));
	model_set_position (App -> figure[FP_L_HAND], glm::vec3 (0.68f, -0.028f, 0.0f));
	model_set_position (App -> figure[FP_R_UPPER_ARM], glm::vec3 (-0.32f, 0.69f, -0.05f));
	model_set_position (App -> figure[FP_R_LOWER_ARM], glm::vec3 (-0.625f, 0.012f, 0.0f));
	model_set_position (App -> figure[FP_R_HAND], glm::vec3 (-0.68f, -0.028f, 0.0f));
	model_set_position (App -> figure[FP_L_UPPER_LEG], glm::vec3 (0.15f, -0.115f, -0.005f));
	model_set_position (App -> figure[FP_L_LOWER_LEG], glm::vec3 (-0.02f, -1.01f, 0.0f));
	model_set_position (App -> figure[FP_L_FOOT], glm::vec3 (0.0f, -1.05f, 0.0f));
	model_set_position (App -> figure[FP_R_UPPER_LEG], glm::vec3 (-0.15f, -0.115f, -0.005f));
	model_set_position (App -> figure[FP_R_LOWER_LEG], glm::vec3 (0.02f, -1.01f, 0.0f));
	model_set_position (App -> figure[FP_R_FOOT], glm::vec3 (0.0f, -1.05f, 0.0f));

	App -> figure[FP_PELVIS] -> bounding_sphere_radius = 0.0f;
	App -> figure[FP_CHEST] -> bounding_sphere_radius = 0.2f;
	App -> figure[FP_HEAD] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_L_UPPER_ARM] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_L_LOWER_ARM] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_L_HAND] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_R_UPPER_ARM] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_R_LOWER_ARM] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_R_HAND] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_L_UPPER_LEG] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_L_LOWER_LEG] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_L_FOOT] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_R_UPPER_LEG] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_R_LOWER_LEG] -> bounding_sphere_radius = 0.1f;
	App -> figure[FP_R_FOOT] -> bounding_sphere_radius = 0.1f;
}

static void render_figure (app* App, glm::mat4 view_matrix) {
	shader_use (App -> figure[0] -> shader_id);
	shader_set_mat4 (App -> figure[0] -> shader_id, "view", view_matrix);
	shader_set_vec3 (App -> figure[0] -> shader_id, "view_direction", App -> camera.cam.front);
	for (unsigned i = 0; i < FP_COUNT; ++i)
		model_render (App -> figure[i]);
}

void app_init (void* memory, platform_api api) {
	app* App = (app*)memory;
	App -> orthographic_camera = false;

	unsigned default_shader = load_shader (api, DEFAULT_VERT_SOURCE, DEFAULT_FRAG_SOURCE);
	unsigned unlit_shader = load_shader (api, UNLIT_VERT_SOURCE, UNLIT_FRAG_SOURCE);

	setup_figure (App, default_shader);

	App -> gizmo = model_new (obj_load (GIZMO), unlit_shader);
	App -> gizmo -> visible = false;
	App -> camera = arc_ball_new (glm::vec3 (0.0f, 0.0f, 0.0f));

	App -> debug_sphere = model_new (obj_load (DEBUG_SPHERE), unlit_shader);

	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);
	update_projection (App, window_width, window_height);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_use (App -> figure[0] -> shader_id);
	shader_set_mat4 (App -> figure[0] -> shader_id, "view", view);
	shader_set_vec3 (App -> figure[0] -> shader_id, "light_direction", glm::vec3 (-1.0f, -1.0f, 0.0f));

	shader_use (App -> gizmo -> shader_id);
	shader_set_mat4 (App -> gizmo -> shader_id, "view", view);

	glEnable (GL_DEPTH_TEST);

	App -> debug = false;
}

void app_update_and_render (void* memory, platform_api api, input in, float dt) {
	app* App = (app*)memory;

	if (api.was_window_resized ()) {
		unsigned window_width, window_height;
		api.get_window_size (&window_width, &window_height);
		update_projection (App, window_width, window_height);
	}

	handle_input (App, api, in, dt);

	glClearColor (BG_COLOR);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_use (App -> gizmo -> shader_id);
	shader_set_mat4 (App -> gizmo -> shader_id, "view", view);
	model_render (App -> gizmo);

	render_figure (App, view);

	if (App -> debug) {
		for (unsigned i = 0; i < FP_COUNT; ++i) {
			App -> debug_sphere -> scale = App -> figure[i] -> bounding_sphere_radius;
			model_set_position (App -> debug_sphere, App -> figure[i] -> world_position);
			model_render (App -> debug_sphere);
		}
	}
}

void app_close (void* memory) {
	app* App = (app*)memory;

	for (unsigned i = 0; i < FP_COUNT; ++i) {
		obj_delete (App -> figure[i] -> obj);
		free (App -> figure[i]);
	}

	obj_delete (App -> gizmo -> obj);
	free (App -> gizmo);
	obj_delete (App -> debug_sphere -> obj);
	free (App -> debug_sphere);
}
