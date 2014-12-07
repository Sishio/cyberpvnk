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
