#ifndef SERVER_PHYSICS_H
	#define SERVER_PHYSICS_H
	#define MAP_BOUNDS_DECREASE_RATE .5
	#define GRAVITY_RATE 9.81 // give or take
	#include "../main.h"
	#include "../util/util_main.h"
	#include "../class/class_main.h"
	#include "../thread/thread_main.h"
	#include "server_class.h"
	#include "thread"
	#include "cstring"
	extern std::vector<client_t*> client;
	extern void physics_engine();
	extern void new_init_coord_t();
	extern void delete_close_coord_t(coord_t*);
	extern void new_init_model_t();
	extern void delete_close_model_t(model_t*);
#endif
