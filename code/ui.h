#if !defined (UI_H)
#define UI_H

#include "glm/gtc/matrix_transform.hpp"

enum ui_state { UI_DEFAULT, UI_HOVER, UI_CLICK };

struct input;

struct ui {
	glm::mat4 projection;
	unsigned VAO;
	unsigned VBO;

	unsigned vertex_count;
	unsigned shader_id;
};

struct ui_button {
	glm::vec2 position;
	glm::vec2 size;
	ui_state state;
};

ui* ui_new (unsigned shader_id);
ui_button* ui_button_new (glm::vec2 position, glm::vec2 size);

void ui_render_rect (ui* UI, glm::vec2 position, glm::vec2 size, glm::vec3 color);
bool ui_do_button (ui* UI, ui_button* b, input in, glm::vec3 default_color, glm::vec3 hover_color, glm::vec3 click_color);

#endif