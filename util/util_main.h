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
	#include "../math/math_main.h"
	#include  "../main.h"
	#include "cstdio"
	#include "vector"
	#include "climits"
	#include "cstdlib"
	#include "string"
	#include "cassert"
	#include "fstream"
	#include "SDL2/SDL.h"
	#ifdef __linux
		#include "fcntl.h"
		#include "sys/types.h"
		#include "unistd.h"
		#include "time.h"
	#elif _WIN32
		#include "windows.h"
	#endif
	#define likely(x) __builtin_expect(x,1)
	#define unlikely(x) __builtin_expect(x,0)
	#define UTIL_SHELL_DELETE 0
	#define UTIL_TIME_MILLISECOND 0
	#define UTIL_TIME_SECOND 1
	class timer_struct_t{
	private:
		long double start_time;
		long double end_time;
		int type;
	public:
		void start_timer();
		void end_timer();
		long double get_time(int);
	};
	extern int util_shell(int,char*);
	extern void ms_sleep(long double);
	extern unsigned long int gen_rand(unsigned int a = UINT_MAX);
	extern short int term_if_true(bool, char*);
	extern short int warn_if_true(bool, char*);
	extern bool long_double_cmp(long double a,long double b,long double degree_of_error = (long double)0.001);
	extern bool probably_equal(int,int);
	extern long double get_time();
	extern bool sign(long double*);
	extern int encrypt(int);
	extern int encrypt(int*,int); // I don't think this will ever be needed
	extern int encrypt(std::vector<int>);
	extern int encrypt(std::vector<int*>);
	extern int encrypt(std::vector<double>);
	extern int encrypt(std::vector<double*>);
	extern int encrypt(std::vector<long double>);
	extern int encrypt(std::vector<long double*>);
	extern int encrypt(std::vector<std::string>);
	extern int encrypt(std::vector<std::string*>);
#endif
