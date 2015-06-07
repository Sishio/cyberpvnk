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
	#include "../class/class_array.h"
	#include "thread"
	#include "cstring"
	struct physics_rules_t{
	private:
		long double acceleration;
		long double mysterious_downward_force; // mysterious downward force
	public:
		array_t array;
		physics_rules_t();
		~physics_rules_t();
		long double get_gravity();
		long double get_acceleration();
	};
	extern void physics_init();
	extern void physics_close();
	extern void physics_engine();
#endif
