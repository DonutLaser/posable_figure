#if !defined (POSABLE_FIGURE_H)
#define POSABLE_FIGURE_H

#include "model.h"
#include "arc_ball.h"

struct input;
struct platform_api;

struct app {
	model figure;
	arc_ball camera;
};

void app_init (void* memory, platform_api platform);
void app_update_and_render (void* memory, platform_api platform, input in, float dt);
void app_close (void* memory);

#endif