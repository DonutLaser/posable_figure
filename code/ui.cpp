#include "ui.h"

#include "shader.h"
#include "gl_extensions.h"
#include <gl/gl.h>

#include "glm/glm.hpp"
#include "platform.h"
#include "texture.h"

#include <stdio.h>

static bool is_point_in_rect (glm::vec4 rect, glm::vec2 point) {
	return point.x >= rect.x && point.x <= rect.x + rect.z &&
		   point.y >= rect.y && point.y <= rect.y + rect.w;
}

ui* ui_new (unsigned shader_id, unsigned texture_shader_id) {
	ui* result = (ui*)malloc (sizeof (ui));
	result -> vertex_count = 6;
	result -> shader_id = shader_id;
	result -> texture_shader_id = texture_shader_id;

	float vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays (1, &result -> VAO);
	glGenBuffers (1, &result -> VBO);

	glBindVertexArray (result -> VAO);

	glBindBuffer (GL_ARRAY_BUFFER, result -> VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (float) * result -> vertex_count * 4, &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void*)0);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void*)(2 * sizeof (float)));
	glEnableVertexAttribArray (1);

	glBindVertexArray (0);

	return result;
}

ui_button* ui_button_new (glm::vec2 position, glm::vec2 size) {
	ui_button* result = (ui_button*)malloc (sizeof (ui_button));
	result -> position = position;
	result -> size = size;
	result -> state = UI_DEFAULT;
	result -> tex = NULL;

	return result;
}

void ui_render_rect (ui* UI, glm::vec2 position, glm::vec2 size, glm::vec3 color) {
	shader_use (UI -> shader_id);

	glm::mat4 model_matrix = glm::mat4 (1.0f);
	model_matrix = glm::translate (model_matrix, glm::vec3 (position, 0.0f));
	model_matrix = glm::scale (model_matrix, glm::vec3 (size, 1.0f));

	shader_set_mat4 (UI -> shader_id, "model", model_matrix);
	shader_set_vec3 (UI -> shader_id, "diffuse_color", color);

	glBindVertexArray (UI -> VAO);
	glDrawArrays (GL_TRIANGLES, 0, UI -> vertex_count);
	glBindVertexArray (0);
}

void ui_render_texture (ui* UI, glm::vec2 position, texture* tex) {
	shader_use (UI -> texture_shader_id);

	glm::mat4 model_matrix = glm::mat4 (1.0f);
	model_matrix = glm::translate (model_matrix, glm::vec3 (position, 0.0f));
	model_matrix = glm::scale (model_matrix, glm::vec3 (tex -> size, 1.0f));

	shader_set_mat4 (UI -> texture_shader_id, "model", model_matrix);
	shader_set_vec3 (UI -> texture_shader_id, "diffuse_color", glm::vec3 (1.0f, 1.0f, 1.0f));

	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, tex -> id);

	glBindVertexArray (UI -> VAO);
	glDrawArrays (GL_TRIANGLES, 0, UI -> vertex_count);
	glBindVertexArray (0);
}

bool ui_do_button (ui* UI, ui_button* b, input in, glm::vec3 default_color, glm::vec3 hover_color, glm::vec3 click_color) {
	bool result = false;
	glm::vec3 final_color = default_color;

	if (is_point_in_rect (glm::vec4 (b -> position, b -> size), glm::vec2 (in.mouse_x, in.mouse_y))) {
		if (b -> state == UI_CLICK)
			final_color = click_color;
		else {
			b -> state = UI_HOVER;
			final_color = hover_color;
		}

		if (in.lmb_down) {
			b -> state = UI_CLICK;
			final_color = click_color;
		}
		else if (in.lmb_up) {
			b -> state = UI_HOVER;
			final_color = hover_color;
			result = true;
		}
 	}

 	if (in.lmb_up) {
 		b -> state = UI_DEFAULT;
 		final_color = default_color;
 	}

 	ui_render_rect (UI, b -> position, b -> size, final_color);
 	if (b -> tex)
	 	ui_render_texture (UI, b -> position, b -> tex);

	return result;
}
