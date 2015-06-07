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
	#define LOOP_CODE_PARTIAL_MT 1
	#define LOOP_CODE_PRINT_THIS_TIME 2
	#define LOOP_CODE_NEVEREND_MT 4
	#define LOOP_PRINT_THIS_TIME LOOP_CODE_PRINT_THIS_TIME
	#define LOOP_PARTIAL_MT LOOP_CODE_PARTIAL_MT
	#define LOOP_NEVEREND_MT LOOP_CODE_NEVEREND_MT
	#include "../util/util_main.h"
	#include "../input/input_main.h"
	#include "future"
	#include "vector"
	#include "thread"
	#include "chrono"
	#include "functional"
	#include "pthread.h"
	struct loop_thread_pool_t{
	private:
		std::future<typename std::result_of<void (*&())()>::type> thread_vector[1024];
		int_ core_count;
		int_ find_empty_thread();
	public:
		loop_thread_pool_t(int_ core_count);
		~loop_thread_pool_t();
		void add(void(*code)()); // the threads run and delete themselves
		void add(void(*code)(void*), void*);
		void wait();
	};
	struct loop_entry_t{
		array_t array;
		loop_entry_t();
		void(*code)();
		std::thread *thread;
		int_ settings;
		bool get_settings(int_);
		void set_settings(int_);
		int_ iteration_skip;
		int_ term;
		std::string name;
		long double start_time;
		long double end_time;
	};
	struct loop_t{
	public:
		array_t array;
		loop_t();
		long double target_rate;
		long double average_rate;
		std::vector<array_id_t> code;
		int_ settings;
		int_ tick;
	};
	extern void loop_add(loop_t*, loop_entry_t*);
	extern void loop_run(loop_t*);
	extern void loop_del(loop_t*, void(*)());
	extern void loop_del(loop_t*, std::string);
	extern loop_entry_t* loop_generate_entry(int_ , std::string, void(*)());
	extern bool infinite_loop();
#endif
