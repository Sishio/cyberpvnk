/*
Czech_mate by Daniel
This file is part of Czech_mate.

Czech_mate is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as published by
the Free Software Foundation, 

Czech_mate is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Czech_mate.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SERVER_PHYSICS_H
	#define SERVER_PHYSICS_H
	#define MAP_BOUNDS_DECREASE_RATE .5
	#define GRAVITY_RATE 9.81 // give or take
	#include "../main.h"
	#include "../util/util_main.h"
	#include "../class/class_main.h"
	#include "../thread/thread_main.h"
	#include "server_thread.h"
	#include "thread"
	#include "cstring"
	extern std::vector<client_t*> client;
	extern void physics_engine();
	extern void new_init_coord_t();
	extern void delete_close_coord_t(coord_t*);
	extern void new_init_model_t();
	extern void delete_close_model_t(model_t*);
#endif
