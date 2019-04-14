#include "posable_figure.h"

#include "glm/glm.hpp"

#include "obj_loader.h"
#include "platform.h"
#include "shader.h"
#include "constants.h"

#include "gl_extensions.h"
#include <gl/gl.h>

static void handle_input (app* App, input in, float dt) {
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

	if (in.scroll_dir != 0)
		arc_ball_zoom (&App -> camera, -in.scroll_dir);
}

void app_init (void* memory, platform_api api) {
	app* App = (app*)memory;

	char* vert_source;
	char* frag_source;
	api.read_file (SHADER_VERT_SOURCE, &vert_source);
	api.read_file (SHADER_FRAG_SOURCE, &frag_source);
	unsigned default_shader = shader_new (vert_source, frag_source);
	free (vert_source);
	free (frag_source);

	material mat = material_new (default_shader);

	OBJ model = obj_load (MODEL_SOURCE);

	App -> figure = model_new (model, mat);
	App -> camera = arc_ball_new (glm::vec3 (0.0f, 0.0f, 0.0f));

	shader_use (App -> figure.mat.shader);
	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);
	glm::mat4 projection = glm::mat4 (1.0f);
	projection = glm::perspective (glm::radians (App -> camera.cam.zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
	shader_set_mat4 (App -> figure.mat.shader, "projection", projection);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_set_mat4 (App -> figure.mat.shader, "view", view);

	shader_set_vec3 (App -> figure.mat.shader, "light_direction", glm::vec3 (0.0f, -1.0f, 0.0f));

	glEnable (GL_DEPTH_TEST);
}

void app_update_and_render (void* memory, platform_api api, input in, float dt) {
	app* App = (app*)memory;

	if (api.was_window_resized ()) {
		shader_use (App -> figure.mat.shader);
		unsigned window_width, window_height;
		api.get_window_size (&window_width, &window_height);
		glm::mat4 projection = glm::mat4 (1.0f);
		projection = glm::perspective (glm::radians (App -> camera.cam.zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
		shader_set_mat4 (App -> figure.mat.shader, "projection", projection);	
	}

	handle_input (App, in, dt);

	glClearColor (BG_COLOR);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_use (App -> figure.mat.shader);
	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_set_mat4 (App -> figure.mat.shader, "view", view);
	shader_set_vec3 (App -> figure.mat.shader, "view_direction", App -> camera.cam.front);

	model_render (&App -> figure);
}

void app_close (void* memory) {
	app* App = (app*)memory;

	obj_delete (App -> figure.obj);
}
