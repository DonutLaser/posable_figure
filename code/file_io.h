#if !defined (FILE_IO_H)
#define FILE_IO_H

#include <windows.h>

#define PATH_MAX	128	

enum write_mode { WM_APPEND, WM_OVERWRITE };
enum open_mode { OM_READ, OM_WRITE, OM_READWRITE };

struct file {
	void* handle;
	char* path; 
	FILETIME last_write_time;

	char* contents;
};

file open_file (const char* path, open_mode mode);
bool read_file (file* f);
bool read_file (const char* path, char** buffer);
bool write_file (file* f, char* text, write_mode = WM_OVERWRITE);
bool write_file (const char* path, char* text, write_mode = WM_OVERWRITE);
void close_file (file* f);

bool was_file_changed (file f);

bool show_save_file_dialog (const char* title, const char* file_type, const char* extension, char** result);
bool show_load_file_dialog (const char* title, const char* file_type, const char* extension, char** result);
bool show_select_folder_dialog (const char* title, char** result);

#endif
