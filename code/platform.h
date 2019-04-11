#if !defined (PLATFORM_H)
#define PLATFORM_H

#define READ_FILE_PROC(name) bool name (const char* file, char** buffer)
typedef READ_FILE_PROC (read_file_proc);
#define GET_WINDOW_SIZE_PROC(name) void name (unsigned* width, unsigned* height)
typedef GET_WINDOW_SIZE_PROC (get_window_size_proc);

struct input {
	int scroll_dir;
	int last_mouse_x;
	int last_mouse_y;
	int mouse_x;
	int mouse_y;

	bool lmb_pressed;
	bool rmb_pressed;
};

struct platform_api {
	read_file_proc* read_file;
	get_window_size_proc* get_window_size;
};

#endif 