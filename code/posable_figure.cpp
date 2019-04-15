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

		shader_use (App -> figure.shader_id);
		shader_set_mat4 (App -> figure.shader_id, "projection", projection);	

		shader_use (App -> gizmo.shader_id);
		shader_set_mat4 (App -> gizmo.shader_id, "projection", projection);
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

void app_init (void* memory, platform_api api) {
	app* App = (app*)memory;
	App -> orthographic_camera = false;

	unsigned default_shader = load_shader (api, DEFAULT_VERT_SOURCE, DEFAULT_FRAG_SOURCE);
	unsigned unlit_shader = load_shader (api, UNLIT_VERT_SOURCE, UNLIT_FRAG_SOURCE);

	OBJ model = obj_load (MODEL_SOURCE);
	OBJ gizmo = obj_load (GIZMO_SOURCE);

	App -> figure = model_new (model, default_shader);
	App -> gizmo = model_new (gizmo, unlit_shader);
	App -> gizmo.visible = false;
	App -> camera = arc_ball_new (glm::vec3 (0.0f, 0.0f, 0.0f));

	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);
	update_projection (App, window_width, window_height);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_use (App -> figure.shader_id);
	shader_set_mat4 (App -> figure.shader_id, "view", view);
	shader_set_vec3 (App -> figure.shader_id, "light_direction", glm::vec3 (-1.0f, -1.0f, 0.0f));

	shader_use (App -> gizmo.shader_id);
	shader_set_mat4 (App -> gizmo.shader_id, "view", view);

	glEnable (GL_DEPTH_TEST);
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
	shader_use (App -> figure.shader_id);
	shader_set_mat4 (App -> figure.shader_id, "view", view);
	shader_set_vec3 (App -> figure.shader_id, "view_direction", App -> camera.cam.front);
	shader_use (App -> gizmo.shader_id);
	shader_set_mat4 (App -> gizmo.shader_id, "view", view);

	model_render (&App -> figure);
	model_render (&App -> gizmo);
}

void app_close (void* memory) {
	app* App = (app*)memory;

	obj_delete (App -> figure.obj);
}
