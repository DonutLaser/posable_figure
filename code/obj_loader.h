#if !defined (OBJ_LOADER_H)
#define OBJ_LOADER_H

struct float_array {
	float* data;
	unsigned capacity;
	unsigned count;
};

struct OBJ {
	float_array vertices;
	float_array uv;
	float_array normals;

	char name[32];
};

OBJ obj_load (const char* path);
void obj_delete (OBJ obj);

#endif