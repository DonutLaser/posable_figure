#if !defined (OBJECT_H)
#define OBJECT_H

#include "model.h"
#include "transform.h"

struct object {
	model* m;
	transform* t;
};

object* object_new ();
object* object_new (model* m);

void object_render (object* obj);

void object_free (object* obj);

#endif