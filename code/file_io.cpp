#include "file_io.h"

#include <stdio.h>
#include <string.h>
#include <Shlobj.h>

DWORD open_modes[] = {
	GENERIC_READ,
	GENERIC_WRITE,
	GENERIC_READ | GENERIC_WRITE
};

file open_file (const char* path, open_mode mode) {
	file result = { };
	result.handle = CreateFile (path, open_modes[mode],
						   		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
						   		OPEN_ALWAYS,
						   		FILE_ATTRIBUTE_NORMAL, NULL);

	result.path = (char*)path;
	GetFileTime (result.handle, NULL, NULL, &result.last_write_time);

	return result;
}

bool read_file (file* f) {
	DWORD size = GetFileSize (f -> handle, NULL);
	f -> contents = (char*)calloc (size + 1, sizeof (char));

	if (!f -> contents) {
		printf ("Unable to allocate memory for file");
		return false;
	}

	DWORD bytes;
	bool success = ReadFile (f -> handle, f -> contents, size + 1, &bytes, NULL);
	f -> contents[size] = '\0';

	return success;
}

bool read_file (const char* path, char** buffer) {
	file f = open_file (path, OM_READ);

	if (!f.handle) {
		printf ("Unable to open the file at path %s\n", path);
		return false;
	}

	bool success = read_file (&f);
	if (success) {
		unsigned size = (unsigned)strlen (f.contents);
		*buffer = (char*)malloc (sizeof (char) * (size + 1));
		strcpy_s (*buffer, size + 1, f.contents);
		(*buffer)[size] = '\0';
	}

	close_file (&f);

	return success;
}

bool write_file (file* f, char* text, write_mode mode) {
	if (mode = WM_OVERWRITE) {
		LARGE_INTEGER offset = { };
		SetFilePointerEx (f -> handle, offset, NULL, FILE_BEGIN);
		SetEndOfFile (f -> handle);
	}

	unsigned size = (unsigned)strlen (text);

	DWORD bytes = 0;
	return WriteFile (f -> handle, text, size, &bytes, NULL);
}

bool write_file (const char* path, char* text, write_mode mode) {
	file f = open_file (path, OM_WRITE);

	bool success = write_file (&f, text, mode);
	close_file (&f);

	return success;
}

void close_file (file* f) {
	CloseHandle (f -> handle);
	free (f -> contents);
}

bool was_file_changed (file f) {
	FILETIME write_time;

	if (GetFileTime (f.handle, NULL, NULL, &write_time))
		return CompareFileTime (&f.last_write_time, &write_time) < 0;

	printf ("Unable to get last write time of the file %s\n", f.path);

	return false;
}

bool show_save_file_dialog (const char* title, const char* file_type, const char* extension, char** result) {
	char* filter = (char*)malloc (sizeof (char) * PATH_MAX);
	unsigned file_type_count = (unsigned)strlen (file_type);
	unsigned extension_count = (unsigned)strlen (extension);

	for (unsigned i = 0; i < file_type_count; ++i)
		filter[i] = file_type[i];

	filter[file_type_count] = ' ';
	filter[file_type_count + 1] = '(';
	filter[file_type_count + 2] = '*';
	filter[file_type_count + 3] = '.';

	for (unsigned i = file_type_count + 4; i < file_type_count + 4 + extension_count; ++i)
		filter[i] = extension[i - file_type_count - 4];

	filter[file_type_count + 4 + extension_count] = ')';
	filter[file_type_count + 5 + extension_count] = '\0';
	filter[file_type_count + 6 + extension_count] = '\0';

	char file_string[PATH_MAX] = "";

	OPENFILENAME open_file_name = { };
	open_file_name.lStructSize = sizeof (OPENFILENAME);
	open_file_name.lpstrFilter = filter;
	open_file_name.lpstrFile = file_string;
	open_file_name.nMaxFile = PATH_MAX;
	open_file_name.lpstrTitle = title;
	open_file_name.Flags = OFN_CREATEPROMPT | OFN_DONTADDTORECENT | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;
	open_file_name.nFileOffset = 0;
	open_file_name.lpstrDefExt = extension;

	bool res = false;
	if (GetSaveFileName (&open_file_name)) {
		unsigned length = (unsigned)strlen (open_file_name.lpstrFile);
		*result = (char*)malloc (sizeof (char) * (length + 1));
		strcpy_s (*result, length + 1, open_file_name.lpstrFile);
		(*result)[length] = '\0';
		res = true;
	}

	free (filter);
	return res;
}

bool show_load_file_dialog (const char* title, const char* file_type, const char* extension, char** result) {
	char* filter = (char*)malloc (sizeof (char) * PATH_MAX);
	unsigned file_type_count = (unsigned)strlen (file_type);
	unsigned extension_count = (unsigned)strlen (extension);

	for (unsigned i = 0; i < file_type_count; ++i)
		filter[i] = file_type[i];

	filter[file_type_count] = ' ';
	filter[file_type_count + 1] = '(';
	filter[file_type_count + 2] = '*';
	filter[file_type_count + 3] = '.';

	for (unsigned i = file_type_count + 4; i < file_type_count + 4 + extension_count; ++i)
		filter[i] = extension[i - file_type_count - 4];

	filter[file_type_count + 4 + extension_count] = ')';
	filter[file_type_count + 5 + extension_count] = '\0';
	filter[file_type_count + 6 + extension_count] = '\0';

	char file_string[PATH_MAX] = "";

	OPENFILENAME open_file_name = { };
	open_file_name.lStructSize = sizeof (OPENFILENAME);
	open_file_name.lpstrFilter = filter;
	open_file_name.lpstrFile = file_string;
	open_file_name.nMaxFile = PATH_MAX;
	open_file_name.lpstrTitle = title;
	open_file_name.Flags = OFN_CREATEPROMPT | OFN_DONTADDTORECENT | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;
	open_file_name.nFileOffset = 0;
	open_file_name.lpstrDefExt = extension;

	bool res = false;
	if (GetOpenFileName (&open_file_name)) {
		unsigned length = (unsigned)strlen (open_file_name.lpstrFile);
		*result = (char*)malloc (sizeof (char) * (length + 1));
		strcpy_s (*result, length + 1, open_file_name.lpstrFile);
		(*result)[length] = '\0';
		res = true;
	}

	free (filter);
	return res;
}

bool show_select_folder_dialog (const char* title, char** result) {
	char display_name[PATH_MAX];

	BROWSEINFO info = { };
	info.pszDisplayName = display_name;
	info.lpszTitle = title;
	info.ulFlags = BIF_USENEWUI;

	bool res = false;
	LPITEMIDLIST folder = SHBrowseForFolder (&info);
	if (folder) {
		SHGetPathFromIDList (folder, display_name);

		unsigned length = (unsigned)strlen (display_name);
		*result = (char*)malloc (sizeof (char) * (length + 1));
		strcpy_s (*result, length + 1, display_name);
		(*result)[length] = '\0';

		res = true;
	}

	return res;
}
