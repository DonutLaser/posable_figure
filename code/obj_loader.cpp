#include "obj_loader.h"
#include "file_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_ARRAY_SIZE	16

struct face {
	unsigned vert;
	unsigned uv;
	unsigned normal;
};

struct face_array {
	face* data;
	unsigned capacity;
	unsigned count;
};

// ARRAY OPERATIONS ------------------------------------------------------------------------
static float_array float_array_new () {
	float_array result = { };
	result.capacity = INITIAL_ARRAY_SIZE;
	result.data = (float*)malloc (sizeof (float) * result.capacity);

	return result;
}

static void float_array_add (float_array* array, float value) {
	if (array -> count == array -> capacity) {
		array -> capacity *= 2;
		array -> data = (float*)realloc (array -> data, sizeof (float) * array -> capacity);
	}

	array -> data[array -> count++] = value;
}

static face_array face_array_new () {
	face_array result = { };
	result.capacity = INITIAL_ARRAY_SIZE;
	result.data = (face*)malloc (sizeof (face) * result.capacity);

	return result;
}

static void face_array_add (face_array* array, face value) {
	if (array -> count == array -> capacity) {
		array -> capacity *= 2;
		array -> data = (face*)realloc (array -> data, sizeof (face) * array -> capacity);
	}

	array -> data[array -> count++] = value;
}

// LOADER -------------------------------------------------------------------------------------
static void skip_line (char** contents) {
	while (*(*contents) != '\n')
		++(*contents);

	++(*contents); // Move to the next character after the new line
}

static void skip_whitespace (char** contents) {
	while (*(*contents) == ' ' || *(*contents) == '\t' ||
		   *(*contents) == '\r' || *(*contents) == '\n') {
		++(*contents);
	}
}

static char* get_next_token (char** contents) {
	skip_whitespace (contents);

	unsigned count = 0;
	char* start = *contents;
	while (*(*contents) != '\0') {
		if (*(*contents) == ' ' || *(*contents) == '\n')
			break;

		++(*contents);
		++count;
	}

	if (count == 0)
		return NULL;

	char* result = (char*)malloc (sizeof (char) * (count + 1));
	strncpy_s (result, count + 1, start, count);
	result[count] = '\0';

	return result;
}

static float parse_float (char* str) {
	return (float)atof (str);
}

OBJ obj_load (const char* path) {
	OBJ result = { };

	char* obj_file;
	if (!read_file (path, &obj_file)) {
		printf ("Unable to read file at %s\n", path);
		return result;
	}

	char* contents = obj_file;

	float_array obj_vertices = float_array_new ();
	float_array obj_uv = float_array_new ();
	float_array obj_normals = float_array_new ();
	face_array obj_faces = face_array_new ();

	char* token = get_next_token (&contents);
	while (token) {
		if (strcmp (token, "#") == 0)
			skip_line (&contents);
		else if (strcmp (token, "o") == 0) {
			free (token);
			token = get_next_token (&contents);

			strcpy_s (result.name, 32, token);
		}
		else if (strcmp (token, "v") == 0) {
			for (unsigned i = 0; i < 3; ++i) {
				free (token);
				token = get_next_token (&contents);

				float_array_add (&obj_vertices, parse_float (token));
			}
		}
		else if (strcmp (token, "vt") == 0) {
			for (unsigned i = 0; i < 2; ++i) {
				free (token);
				token = get_next_token (&contents);

				float_array_add (&obj_uv, parse_float (token));
			}
		}
		else if (strcmp (token, "vn") == 0) {
			for (unsigned i = 0; i < 3; ++i) {
				free (token);
				token = get_next_token (&contents);

				float_array_add (&obj_normals, parse_float (token));
			}
		}
		else if (strcmp (token, "f") == 0) {
			for (unsigned i = 0; i < 3; ++i) {
				free (token);
				token = get_next_token (&contents);
				char* modified_token = token;

				face new_face = { };
				char* int_token = strtok (modified_token, "/");
				new_face.vert = (unsigned)atoi (int_token);

				int_token = strtok (NULL, "/");
				new_face.uv = (unsigned)atoi (int_token);

				int_token = strtok (NULL, "/");
				new_face.normal = (unsigned)atoi (int_token);

				face_array_add (&obj_faces, new_face);
			}
		}

		free (token);
		token = get_next_token (&contents);
	}

	result.vertices = float_array_new ();
	result.uv = float_array_new ();
	result.normals = float_array_new ();

	for (unsigned i = 0; i < obj_faces.count; ++i) {
		face f = obj_faces.data[i];
		for (unsigned i = 0; i < 3; ++i)
			float_array_add (&result.vertices, obj_vertices.data[(f.vert - 1) * 3 + i]);

		for (unsigned i = 0; i < 2; ++i)
			float_array_add (&result.uv, obj_uv.data[(f.uv - 1) * 2 + i]);

		for (unsigned i = 0; i < 3; ++i)
			float_array_add (&result.normals, obj_normals.data[(f.normal - 1) * 3 + i]);
	}

	free (obj_vertices.data);
	free (obj_uv.data);
	free (obj_normals.data);
	free (obj_faces.data);

	free (obj_file);
	return result;
}

void obj_delete (OBJ obj) {
	free (obj.vertices.data);
	free (obj.uv.data);
	free (obj.normals.data);
}
