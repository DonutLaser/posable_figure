#if !defined (OBJ_LOADER_H)
#define OBJ_LOADER_H

struct float_array {
	float* data;
	unsigned capacity;
	unsigned count;
};

struct vert_group {
	unsigned material_id;
	unsigned start_index;
	unsigned end_index;
};

struct vert_group_array {
	vert_group* data;
	unsigned capacity;
	unsigned count;
};

struct MTL_mat {
	char name[32];
	float ambient_color[3];
	float diffuse_color[3];
	float specular_color[3];
	float specular_exponent;
};

struct MTL {
	MTL_mat* materials;
	unsigned count;
};

struct OBJ {
	char name[32];

	float_array vertices;
	float_array uv;
	float_array normals;

	vert_group_array vertex_groups;

	MTL material_library;
};

OBJ obj_load (const char* path);
void obj_delete (OBJ obj);

#endif