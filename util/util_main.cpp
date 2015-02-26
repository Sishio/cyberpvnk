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
#include "util_main.h"

bool once_per_second = false;

int util_shell(int function, std::string parameter){ // Keeps most of the OS pre-processor code isolated from the rest of the program
	int return_value = 0;
	std::string command = "";
	switch(function){
	case UTIL_SHELL_DELETE:
		#ifdef __linux
			command = "rm -r " + parameter;
		#elif _WIN32
			command = "del " + parameter;
		#endif
                return_value = system(command.c_str());
		break;
	default:
		return_value = -1;
		break;
	}
	return return_value;
}

void ms_sleep(long double ms){
	#ifdef __linux
		usleep((unsigned int)(1000*ms));
	#elif USE_SDL
		SDL_Delay(ms);
	#elif _WIN32
		ms += .5;
		Sleep((int)ms); // I think this is correct
	#endif
}

static unsigned long int old_rand = 2;

unsigned long int gen_rand(unsigned int a){ // range, from 0 to a
	srand((unsigned int)time(NULL)*(unsigned int)old_rand);
	unsigned long int return_value = rand()%a;
	if(return_value == 0){
		return_value++;
	}
	old_rand = return_value;
	return return_value;
}

short int term_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,true)){
		if(details != NULL) printf("'%s' term parameter failed\n",details);
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
	if(__builtin_expect(a,true)){
		if(details != NULL) printf("'%s' warn parameter failed\n",details);
	}
	return 0;
}

long double get_time(){
	#ifdef _WIN32
		return SDL_GetTicks()/1000; // piece of crap timer
	#elif __linux // possibly OS X?
		timespec a;
		clock_gettime(CLOCK_MONOTONIC, &a);
		return a.tv_sec + ((long double)a.tv_nsec/(long double)1000000000.0);
	#endif
}

bool sign(long double *a){
	if(*a > 0){
		return true;
	}
	return false;
}

int encrypt(int *a, int size){
	int average = 0;
	for(int i = 0;i < size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

std::string wrap(char *start, std::string data, char *end){
	return (std::string)start + data + (std::string)end;
}

bool check_for_parameter(const std::string a, int argc_, char **argv_){
	for(int i = 0;i < argc_;i++){
		if(std::string(argv_[i]) == a){
			return true;
		}
	}
	return false;
}

void switch_values(void *a, void *b){
	void *c = a;
	a = b;
	b = c;
}

void switch_values(void **a, void **b){
	void **c = a;
	a = b;
	b = c;
}

static long double old_time = 0;
static long double once_per_second_time = 0;

void once_per_second_update(){
	const long double curr_time = get_time();
	once_per_second_time += curr_time-old_time;
	old_time = curr_time;
	if(once_per_second_time >= 1){
		once_per_second_time = 0;
		once_per_second = true;
	}else{
		once_per_second = false;
	}
}

void sanatize_input(std::string *tmp){
	for(int i = 0;i < 31;i++){ // reserved for networking and data parsing
		unsigned long int position;
		if(unlikely((position = tmp->find_first_of(std::to_string((char)i))) != std::string::npos)){
			tmp->erase(i, 1);
		}
	}
}

void update_progress_bar(long double percent_complete){
	const unsigned long int maximum_size_of_bar = 20;
	const unsigned long int number_of_item = percent_complete/(100/maximum_size_of_bar);
	printf("\r");
	std::string output = "[";
	for(unsigned long int i = 0;i < maximum_size_of_bar;i++){
		if(i <= number_of_item){
			output += "-";
		}else{
			output += " ";
		}
	}
	if(output != "\r"){
		output[output.size()-1] = '>';
	}
	output += "]";
	printf("%s %Lf\%%", output.c_str(), percent_complete);
}
