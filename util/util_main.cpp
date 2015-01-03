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

void s_sleep(long double s){
	ms_sleep(1000*s);
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

unsigned long int gen_rand(unsigned int a){ // range, from 0 to a
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;
	unsigned long t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;
	unsigned long int z_a = z%a;
	if(unlikely(z_a == 0)){
		z_a++;
	}
	return z_a;
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

bool double_cmp(long double a,long double b,long double degree_of_error){
	if(a-degree_of_error < b){
		if(a+degree_of_error > b){
			return true;
		}
	}
	return false;
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

int encrypt(std::vector<int> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<int*> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+*(a[i]))/2;
	}
	return average;
}

int encrypt(std::vector<double> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<double*> a){
	const unsigned int a_size = a.size();
	long double average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+*(a[i]))/2;
	}
	return average;
}

int encrypt(std::vector<long double> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<long double*> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+*(a[i]))/2;
	}
	return average;
}

int encrypt(std::vector<std::string> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		const unsigned int a_i_size = a[i].size();
		for(unsigned int n = 0;n < a_i_size;n++){
			average = (average+a[i][n])/2;
		}
	}
	return average;
}

int encrypt(std::vector<std::string*> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		const unsigned int a_i_size = a[i]->size();
		for(unsigned int n = 0;n < a_i_size;n++){
			average = (average+(*a[i])[n])/2;
		}
	}
	return average;
}

std::vector<std::string> pull_items_data(char *c, std::string a, char *d){
	std::vector<std::string> b;
	while(true){
		const size_t start_start = a.find_first_of(c);
		const size_t end_start = a.find_first_of(d);
		const size_t start_end = start_start + strlen(c);
		const size_t end_end = end_start + strlen(d);
		bool conditional[8] = {false};
		conditional[4] = start_start == std::string::npos;
		conditional[5] = end_start == std::string::npos;
		if(conditional[4] || conditional[5]){ // escape condition
			break;
		}
		conditional[0] = start_start == end_start && start_start != std::string::npos;
		conditional[1] = start_end == end_end && start_start != std::string::npos;
		conditional[2] = start_start > end_start;
		conditional[3] = start_end > end_end;
		if(conditional[0] || conditional[1] || conditional[2] || conditional[3]){
			printf("An impossible condition has been met\n");
			for(unsigned int i = 0;i < 4;i++){
				printf("%d\n",conditional[i]);
			}
			printf("start_start:%zu\tend_start:%zu\tstart_end:%zu\tend_end:%zu\n",start_start, end_start, start_end, end_end);
			assert(false);
		}
		std::string z = a.substr(start_end,(end_start-start_end));
		b.push_back(z);
		if(end_end == a.size()){
			break;
		}
		a = a.substr(end_end+1,a.size());
	}
	return b;
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

void switcH_values(void *a, void *b){
	void *c = a;
	a = b;
	b = c;
}

void switch_values(void **a, void **b){
	void **c = a;
	a = b;
	b = c;
}

