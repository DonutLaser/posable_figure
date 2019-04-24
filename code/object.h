#if !defined (OBJECT_H)
#define OBJECT_H

#include "model.h"
#include "transform.h"
#include "collider.h"

struct object {
	model* m;
	transform* t;
	collider* c;
};

object* object_new ();
object* object_new (model* m);

void object_set_position (object* obj, glm::vec3 position);
void object_set_rotation (object* obj, glm::vec3 rotation);

void object_render (object* obj);

void object_free (object* obj);

#endif