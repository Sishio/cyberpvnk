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
	#define LOOP_CODE_PARTIAL_MT 0 // this runs the code once and stops
	#define LOOP_PRINT_THIS_TIME 1
	#define LOOP_CODE_NEVEREND_MT 2 // threads are stored across iterations
	#include "../util/util_main.h"
	#include "../input/input_main.h"
	#include "vector"
	#include "thread"
	struct loop_entry_t{
		loop_entry_t();
		void(*code)();
		int iteration_skip;
		bool term;
		std::string name;
	};
	struct loop_t{
		loop_t();
		long double target_rate;
		long double average_rate;
		std::string name;
		std::vector<loop_entry_t> code;
		std::vector<std::thread*> neverend_threads;
		int settings;
	};
	extern void loop_add(loop_t*, std::string, void(*)());
	extern void loop_run(loop_t*, int*);
	extern void loop_del(loop_t*, void(*)());
	extern bool infinite_loop();
	extern bool terminate;
#endif
