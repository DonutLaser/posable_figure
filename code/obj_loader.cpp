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

static char* get_folder (const char* path) {
	unsigned size = (unsigned)strlen (path);

	unsigned count_to_cut = 0;
	for (unsigned i = size - 1; size >= 0; --i) {
		if (path[i] == '/')
			break;

		++count_to_cut;
	}

	char* result = (char*)malloc (sizeof (char) * ((size - count_to_cut) + 1));
	for (unsigned i = 0; i < size - count_to_cut; ++i)
		result[i] = path[i];

	result[size - count_to_cut] = '\0';
	return result;
}

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

static vert_group_array vert_group_array_new () {
	vert_group_array result = { };
	result.capacity = INITIAL_ARRAY_SIZE;
	result.data = (vert_group*)malloc (sizeof (vert_group) * result.capacity);

	return result;
}

static void vert_group_array_add (vert_group_array* array, vert_group value) {
	if (array -> count == array -> capacity) {
		array -> capacity *= 2;
		array -> data = (vert_group*)realloc (array -> data, sizeof (vert_group) * array -> capacity);
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

static MTL parse_mtllib (const char* path) {
	MTL result = { };

	char* mtl_file;
	if (!read_file (path, &mtl_file)) {
		printf ("Unable to read file at %s\n", path);
		return result;
	}

	char* contents = mtl_file;

	int current_material = -1;
	char* token = get_next_token (&contents);
	while (token) {
		if (strcmp (token, "#") == 0) {
			free (token);
			token = get_next_token (&contents);
			if (strcmp (token, "Material") == 0) {
				for (unsigned i = 0; i < 2; ++i) {
					free (token);
					token = get_next_token (&contents);
				}

				result.materials = (MTL_mat*)malloc (sizeof (MTL_mat) * atoi (token));
			}
			else
				skip_line (&contents);
		}
		else if (strcmp (token, "newmtl") == 0) {
			++current_material;
			++result.count;

			free (token);
			token = get_next_token (&contents);

			strcpy_s (result.materials[current_material].name, 32, token);
		}
		else if (strcmp (token, "Ns") == 0) {
			free (token);
			token = get_next_token (&contents);
			result.materials[current_material].specular_exponent = parse_float (token);
		}
		else if (strcmp (token, "Ka") == 0) {
			for (unsigned i = 0; i < 3; ++i) {
				free (token);
				token = get_next_token (&contents);

				result.materials[current_material].ambient_color[i] = parse_float (token);
			}
		}
		else if (strcmp (token, "Kd") == 0) {
			for (unsigned i = 0; i < 3; ++i) {
				free (token);
				token = get_next_token (&contents);

				result.materials[current_material].diffuse_color[i] = parse_float (token);
			}
		}
		else if (strcmp (token, "Ks") == 0) {
			for (unsigned i = 0; i < 3; ++i) {
				free (token);
				token = get_next_token (&contents);

				result.materials[current_material].specular_color[i] = parse_float (token);
			}
		}

		free (token);
		token = get_next_token (&contents);
	}

	free (mtl_file);
	return result;
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

	result.vertex_groups = vert_group_array_new ();

	int current_vert_group = -1;
	int vert_index = 0;
	char* token = get_next_token (&contents);
	while (token) {
		if (strcmp (token, "#") == 0)
			skip_line (&contents);
		else if (strcmp (token, "o") == 0) {
			free (token);
			token = get_next_token (&contents);

			strcpy_s (result.name, 32, token);
		}
		else if (strcmp (token, "mtllib") == 0) {
			free (token);
			token = get_next_token (&contents);

			char* folder = get_folder (path);
			char mtl_path[MAX_PATH];
			mtl_path[0] = '\0';
			strcat (mtl_path, folder);
			strcat (mtl_path, token);

			result.material_library = parse_mtllib (mtl_path);

			free (folder);
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
		else if (strcmp (token, "usemtl") == 0) {
			if (current_vert_group != -1)
				result.vertex_groups.data[current_vert_group].end_index = vert_index - 1;

			++current_vert_group;

			vert_group new_group = { };
			new_group.start_index = vert_index;

			free (token);
			token = get_next_token (&contents);
			for (unsigned i = 0; i < result.material_library.count; ++i) {
				if (strcmp (result.material_library.materials[i].name, token) == 0) {
					new_group.material_id = i;
					break;
				}
			}

			vert_group_array_add (&result.vertex_groups, new_group);
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

			vert_index += 3;
		}

		free (token);
		token = get_next_token (&contents);
	}

	result.vertex_groups.data[current_vert_group].end_index = vert_index - 1;

	result.vertices = float_array_new ();
	result.uv = float_array_new ();
	result.normals = float_array_new ();

	for (unsigned i = 0; i < obj_faces.count; ++i) {
		face f = obj_faces.data[i];
		for (unsigned j = 0; j < 3; ++j)
			float_array_add (&result.vertices, obj_vertices.data[(f.vert - 1) * 3 + j]);

		for (unsigned j = 0; j < 2; ++j)
			float_array_add (&result.uv, obj_uv.data[(f.uv - 1) * 2 + j]);

		for (unsigned j = 0; j < 3; ++j)
			float_array_add (&result.normals, obj_normals.data[(f.normal - 1) * 3 + j]);
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
	free (obj.vertex_groups.data);
	free (obj.material_library.materials);
}
