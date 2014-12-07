#include "util_main.h"

static unsigned long x=123456789, y=362436069, z=521288629;

int util_shell(int function,char* parameter){ // Keeps most of the OS pre-processor code isolated from the rest of the program
	int return_value = 0;
	std::string command = "";
	switch(function){
	case UTIL_SHELL_DELETE:
		#ifdef __linux
			command = "rm -r " + (std::string)parameter;
		#elif _WIN32
			command = "del " + (char*)parameter;
		#endif
		return_value = system((char*)command.c_str());
		break;
	default:
		return_value = -1;
		break;
	}
	return return_value;
}

void ms_sleep(double ms){
	#ifdef __linux
		usleep((unsigned int)(1000*ms));
	#elif USE_SDL
		SDL_Delay(ms);
	#elif _WIN32
		ms += .5;
		Sleep((int)ms); // I think this is correct
	#endif
}

unsigned long int gen_rand(unsigned int a){ // range, from 0 to a
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;
	unsigned long t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;
	return z%a;
}

short int term_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,true)){
		if(details != NULL) printf("%s term_parameter failed\n",details);
		#ifndef NDEBUG
			assert(false);
		#else
			exit(0);
		#endif
		return TERMINATE;
	}
	return 0;
}

short int warn_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,0)){
		if(details != NULL) printf("%s warn parameter failed\n",details);
	}
	return 0;
}

bool double_cmp(double a,double b,double degree_of_error){
	if(a-degree_of_error < b){
		if(a+degree_of_error > b){
			return true;
		}
	}
	return false;
}

double get_time(){
	#ifdef _WIN32
		return SDL_GetTicks()/1000; // piece of crap timer, needs sleep
	#elif __linux // possibly OS X?
		timespec a;
		clock_gettime(CLOCK_MONOTONIC, &a); // greatest timer in the world, does not need any sleep
		return (double)a.tv_nsec/(double)1000000000.0;
	#endif
}

bool sign(double *a){
	if(*a > 0){
		return true;
	}
	return false;
}