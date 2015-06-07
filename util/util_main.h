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
#ifndef UTIL_H // this is embeded enough for it to not be modular
	#define UTIL_H
	#define likely(x) __builtin_expect(x,1)
	#define unlikely(x) __builtin_expect(x,0)
	#define UTIL_SHELL_DELETE 0
	#define UTIL_TIME_MILLISECOND 0
	#define UTIL_TIME_SECOND 1
	#define CHECK_BIT(var, pos) ((var & ( 1 << pos )) >> pos)
	#define SET_BIT(var,pos,val) (var ^= (-val ^ var) & (1 << pos))
	#define FLIP_BIT(var, pos) (var ^= (1 << pos))	
	#define PRINTF_VITAL 0
	#define PRINTF_ERROR 1
	#define PRINTF_UNLIKELY_WARN 2
	#define PRINTF_LIKELY_WARN 3
	#define PRINTF_STATUS 4
	#define PRINTF_DEBUG 5
	#define PRINTF_SPAM 6
	#define NEW_NULLPTR_ERROR "ERROR: Couldn't allocate data\n"
	#include "../math/math_main.h"
	#include  "../main.h"
	#include "cstdio"
	#include "iostream"
	#include "mutex"
	#include "vector"
	#include "climits"
	#include "cstdlib"
	#include "string"
	#include "cassert"
	#include "fstream"
	#include "SDL2/SDL.h"
	#include "util_conversion.h"
	#include "chrono"
	#include "thread"
	#ifdef __linux
		#include "fcntl.h"
		#include "sys/types.h"
		#include "unistd.h"
		#include "time.h"
	#elif _WIN32
		#include "windows.h"
	#endif
	#include "../class/class_array.h"
	struct sort_t{
		void *pointer;
		int_ value;
	};
	class timer_struct_t{
	private:
		long double start_time;
		long double end_time;
		int_ type;
	public:
		void start_timer();
		void end_timer();
		long double get_time(int_);
	};
	#define FUNCTION_LOCK_PARAMETER_ARRAY 1
	#define FUNCTION_LOCK_ALL_ARRAYS 2
	#define FUNCTION_NO_CHECK_FOR_PARAMETER_NULLPTR 4
	#define FUNCTION_NO_CHECK_FOR_PARAMETER_ITEMS_NULLPTR 8
	#define FUNCTION_NO_CHECK_FOR_ANY_NULLPTR 12
	extern int_ argc_;
	extern char** argv_;
	extern void update_progress_bar(long double);
	extern bool once_per_second;
	extern void once_per_second_update();
	extern std::string wrap(char *start, std::string data, char *end);
	extern std::vector<std::string> pull_items_data(char*, std::string, char*);
	extern int_ util_shell(int_, std::string);
	extern void ms_sleep(long double);
	extern uint_ gen_rand(uint_ a = 0);
	extern int_ term_if_true(bool, char*);
	extern int_ warn_if_true(bool, char*);
	extern bool probably_equal(int_,int_);
	extern long double get_time();
	extern bool sign(long double*);
	extern bool check_for_parameter(const std::string, int_, char**);
	extern void switch_values(void*, void*);
	extern void switch_values(void**, void**);
	extern void sorting_algorithm(std::vector<void*>, int_);
	extern void sanatize_input(std::string*);
	extern void load_file_to_string(std::string*, std::string);
	extern void positive_to_negative_trick(unsigned char **, int_);
	extern void negative_to_positive_trick(std::string*);
	extern bool quicksort_search(uint_*, uint*, int_, int_);
	extern bool valid_int(int_);
	extern array_id_t strip_id(array_id_t);
	extern int_ printf_(std::string, int_);
	extern array_id_t scramble_id(array_id_t);
	extern void hanging_debug();
	extern long double get_last_time();
	extern void misc_init();
	extern void throw_if_nullptr(void* tmp);
#endif
