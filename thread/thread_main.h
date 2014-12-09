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
#ifndef THREAD_MAIN_H
	#define THREAD_MAIN_H
	#include "../util/util_main.h"
	#include "cstdio"
	#include "cassert"
	#define THREAD_MAX_THREADS 64
	#define USE_MT 1
		#define USE_SDL_THREADS 1
	#ifdef USE_SDL_THREADS
		#ifdef __linux
			#include "SDL2/SDL.h"
		#elif _WIN32
			#include "SDL2.h"
		#endif
	#elif __linux
		#include "pthread.h"
		#include "unistd.h"
	#endif
	struct thread_rules_t{
		int thread_count;
	};
	class thread_t{
	private:
		thread_rules_t rules;
		SDL_Thread **thread;
	public:
		void blank();
		void init(int,char**);
		int run(SDL_ThreadFunction,void*);
		void wait(int);
		void close();
	};
#endif
