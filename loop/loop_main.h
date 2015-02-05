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

#ifndef LOOP_MAIN_H
	#define LOOP_MAIN_H
	#define LOOP_CODE_MT 0
	#include "../util/util_main.h"
	#include "vector"
	#include "thread"
	struct loop_t{
		std::vector<void(*)()> code;
	};
	extern void loop_add(loop_t*, void(*)());
	extern void loop_run(loop_t*, int settings = 0);
	extern void loop_del(loop_t*, void(*)());
#endif
