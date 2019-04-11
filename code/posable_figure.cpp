#include "posable_figure.h"

#include "glm/glm.hpp"

#include "obj_loader.h"
#include "platform.h"
#include "shader.h"
#include "constants.h"

#include "gl_extensions.h"
#include <gl/gl.h>

#define SHADER_VERT_SOURCE		"W:/posable_figure/data/shaders/default.vert"
#define SHADER_FRAG_SOURCE		"W:/posable_figure/data/shaders/default.frag"
#define MODEL_SOURCE			"W:/posable_figure/data/models/obj_test.obj"

static void handle_input (app* App, input in, float dt) {
	if (in.rmb_pressed) {
		int mouse_delta_x = in.last_mouse_x - in.mouse_x;
		int mouse_delta_y = in.mouse_y - in.last_mouse_y;

		App -> camera.yaw += mouse_delta_x * ARC_BALL_SPEED * ARC_BALL_SENSITIVITY;
		App -> camera.pitch += mouse_delta_y * ARC_BALL_SPEED * ARC_BALL_SENSITIVITY;

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
	mat.diffuse_color = glm::vec3 (DIFFUSE_COLOR);

	OBJ model = obj_load (MODEL_SOURCE);

	App -> figure = model_new (model.vertices.data, model.vertices.count, model.uv.data, model.uv.count, model.normals.data, model.normals.count, mat);
	App -> camera = arc_ball_new (glm::vec3 (0.0f, 0.0f, 0.0f));

	shader_use (App -> figure.mat.shader);
	shader_set_vec3 (default_shader, "diffuse_color", mat.diffuse_color);

	unsigned window_width, window_height;
	api.get_window_size (&window_width, &window_height);
	glm::mat4 projection = glm::mat4 (1.0f);
	projection = glm::perspective (glm::radians (App -> camera.cam.zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
	shader_set_mat4 (App -> figure.mat.shader, "projection", projection);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_set_mat4 (App -> figure.mat.shader, "view", view);

	obj_delete (model);

	glEnable (GL_DEPTH_TEST);
}

void app_update_and_render (void* memory, platform_api platform, input in, float dt) {
	app* App = (app*)memory;

	handle_input (App, in, dt);

	glClearColor (BG_COLOR);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera_get_view_matrix (&App -> camera.cam);
	shader_set_mat4 (App -> figure.mat.shader, "view", view);

	model_render (&App -> figure);
}
