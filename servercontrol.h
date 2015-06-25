#include "playerstate.h"

struct ConnectionState
{
	ConnectionState()
		:has_scene(false), last_update(0) { player = NULL; }
	void* player;
	bool has_scene;
	unsigned int last_update;
};
extern bool server;
extern bool keyb_input;
extern unsigned int frame;

#define TICK(a, b) (!((frame + (a)) % (b)))