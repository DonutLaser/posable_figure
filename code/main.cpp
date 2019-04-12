#include "win32_window.h"
#include "file_io.h"
#include <stdio.h>

#include "platform.h"
#include "posable_figure.h"
#include "constants.h"

static win32_window* global_window;

static void platform_get_window_size (unsigned* width, unsigned* height) {
	POINT size = get_window_size (global_window);
	*width = size.x;
	*height = size.y;
}

static bool platform_was_window_resized () {
	bool result = global_window -> resized;
	global_window -> resized = false;

	return result; 
}

GUI_MAIN {
	global_window = window_new ("Posable Figure", WINDOW_WIDTH, WINDOW_HEIGHT);

	void* app_memory = malloc (sizeof (char) * 1024 * 1024 * 5); // 5 MB

	platform_api api = { };
	api.read_file = read_file;
	api.get_window_size = platform_get_window_size;
	api.was_window_resized = platform_was_window_resized;

	app_init (app_memory, api);

	input in = { };
	float dt = 0.0f;
	float last_frame_time = 0.0f;
	while (window_update (global_window)) {
		float current_frame_time = (float)get_time ();
		dt = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;

		in.last_mouse_x = global_window -> input -> last_mouse_x;
		in.last_mouse_y = global_window -> input -> last_mouse_y;
		in.mouse_x = global_window -> input -> mouse_x;
		in.mouse_y = global_window -> input -> mouse_y;
		in.lmb_pressed = is_key_pressed (global_window, KEY_LMB);
		in.rmb_pressed = is_key_pressed (global_window, KEY_RMB);

		in.num1_down = is_key_down (global_window, KEY_NUM1);
		in.num3_down = is_key_down (global_window, KEY_NUM3);
		in.num7_down = is_key_down (global_window, KEY_NUM7);

		in.scroll_dir = global_window -> input -> mouse_scroll_direction;

		app_update_and_render (app_memory, api, in, dt);

		window_render (global_window);
	}

	app_close (app_memory);

	window_free (global_window);
	free (global_window);
	free (app_memory);
	return 0;
};