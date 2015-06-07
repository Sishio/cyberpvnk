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
#include "../render/render_main.h"

bool once_per_second = false;

std::string get_ptr(void* a){
	char data[512];
	snprintf(data, 512, "%p", a);
	return std::string(data);
}

int_ util_shell(int_ function, std::string parameter){ // Keeps most of the OS pre-processor code isolated from the rest of the program
	int_ return_value = 0;
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

void ms_sleep(long double ms_){
	#ifdef __linux
		//timespec time;
		//if(unlikely(ms_ > 999999999)){
		//	time.tv_sec = ms_/1000000000;
		//}else{
		//	time.tv_nsec = ms_*1000000;
		//}
		//nanosleep(&time, nullptr);
		SDL_Delay(ms_);
	#else
		SDL_Delay(ms_);
	#endif
}

#define G_RANDOM 1
//#define CPP11_RANDOM
//#define LINUX_RAND

#ifdef LINUX_RAND
std::ifstream random_stream("/dev/random");
/*
I should be able to change the amount of entropy the kernel has, right?
*/
#endif

uint_ gen_seed(){
	std::ifstream random_stream("/dev/random");
	uint_ retval = 0;
	char retval_[8];
	random_stream.get(retval_, 8);
	for(uint_ i = 0;i < 8;i++){
		retval |= (retval_[i] << 8*i);
	}
	std::cout << "Random: " << retval << std::endl;
	std::cout << "Random binary: " << gen_binary(retval) << std::endl;
	random_stream.close();
	return retval | 1;
}

uint_ r = gen_seed();

uint_ gen_rand(uint_ a){ // range, from 1 to a
	#ifdef CPP11_RANDOM
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> cpp11_random(1, ~((uint_)0));
		return cpp11_random(gen);
	#elif defined(G_RANDOM) // /g/ No. 48126030
		r += 0xE41682A9;
		r ^= 0xE41682A9;
		r = (r >> 1) | (r << 31) | 1;
		return r;
	#elif defined(LINUX_RAND)
		uint_ retval = 0;
		char retval_[8];
		random_stream.get(retval_, 8);
		for(uint_ i = 0;i < 8;i++){
			retval |= (retval_[i] << 8*i);
		}
		std::cout << "Random: " << retval << std::endl;
		return retval | 1;
	#endif
}

int_ term_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,true)){
		if(details != NULL) printf_("'" + std::string(details) + "' term parameter failed.", PRINTF_VITAL);
		hanging_debug();
	}
	return 0;
}

int_ warn_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,true)){
		if(details != NULL) printf("'%s' warn parameter failed\n",details);
	}
	return 0;
}

long double last_time = 0;

long double get_time(){
	long double retval = 0;
	#ifdef _WIN32
		retval = (last_time = SDL_GetTicks()/1000); // piece of crap timer
	#elif __linux // possibly OS X?
		timespec a;
		clock_gettime(CLOCK_MONOTONIC, &a);
		retval = (last_time = a.tv_sec + ((long double)a.tv_nsec/(long double)1000000000.0));
	#endif
	last_time = retval;
	return retval;
}

long double get_last_time(){
	if(last_time == 0){
		last_time = get_time();
	}
	return last_time;
}

bool sign(long double *a){
	if(*a > 0){
		return true;
	}
	return false;
}

int_ encrypt(int_ *a, int_ size){
	int_ average = 0;
	for(int_ i = 0;i < size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

std::string wrap(char *start, std::string data, char *end){
	return (std::string)start + data + (std::string)end;
}

bool check_for_parameter(const std::string a, int_ argc_, char **argv_){
	for(int_ i = 0;i < argc_;i++){
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
	for(int_ i = 0;i < 31;i++){ // reserved for networking and data parsing
		uint_ position;
		if(unlikely((position = tmp->find_first_of(std::to_string((char)i))) != std::string::npos)){
			tmp->erase(i, 1);
		}
	}
}

void update_progress_bar(long double percent_complete){
	const uint_ maximum_size_of_bar = 20;
	const uint_ number_of_item = percent_complete/(100/maximum_size_of_bar);
	printf("\r");
	std::string output = "[";
	for(uint i = 0;i < maximum_size_of_bar;i++){
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
	printf("%s %Lf %%", output.c_str(), percent_complete);
}

void positive_to_negative_trick(unsigned char** tmp_char, int_ size){
	for(int_ i = 0;i < size;i++){
		if((*tmp_char)[i] >= 128){
			(*tmp_char)[i] *= -1;
			(*tmp_char)[i] -= 128;
		}
	}
}

void negative_to_positive_trick(std::string *tmp){
	const uint_ size = tmp->size();
	for(uint_ i = 0;i < size;i++){
		if((*tmp)[i] < 0){
			(*tmp)[i] *= -1;
			(*tmp)[i] += 128;
		}
	}
}

bool quicksort_search(uint_ *i, uint_ *inc, int_ current_value, int_ target_value){
	if(*inc == 0){
		return true;
	}else if(current_value > target_value){
		*i -= *inc;
		*inc /= 2;
		return false;
	}else if(current_value < target_value){
		*i += *inc;
		*inc /= 2;
		return false;
	}else return true;
}

bool valid_int(int_ a){
	return a != DEFAULT_INT_VALUE;
}

std::string gen_binary(array_id_t a){
	std::string return_;
	for(uint_ i = 0;i <= 32;i++){
		return_ += (CHECK_BIT(a, i) == 1) ? "1" : "0";
	}
	return return_;
}

array_id_t strip_id(array_id_t id){
	return id & 0x00000000FFFFFFFF;
}

array_id_t scramble_id(array_id_t id){
	return (id | (gen_rand() << 32));
}

int_ print_level = -1;
int_ abort_print_level = -1;

int_ printf_(std::string data_to_print, int_ status){
	assert(print_level != -1);	
	if(status <= print_level){
		std::cout << data_to_print;
	}
	assert(status > abort_print_level);
	return 0;
}

void hanging_debug(){
	while(check_signal(SIGNAL_QUIT_LOOP) == false){
		ms_sleep(1000);
	}
}

void free_ram(){
	printf_("ERROR: Ran out of RAM. DO SOMETHING\n", PRINTF_VITAL);
}

void test_random(){
	uint_ old = 0;
	for(uint_ i = 0;i < 8192;i++){
		uint_ new_ = gen_rand();
		assert(old != new_);
		old = new_;
	}
}

void test_ids(){
	array_id_t id = ARRAY_VECTOR_SIZE;
	array_id_t id_ = id;
	id = scramble_id(id);
	std::cout << "Scrambled ID: " << id << std::endl;
	id = strip_id(id);
	std::cout << "Re-stripped ID: " << id << std::endl;
	if(id != id_){
		std::cout << id << " " << id_ << std::endl;
		assert(false);
	}
}

void set_print_levels(){
	if(check_for_parameter("--debug", argc_, argv_)){
		print_level = PRINTF_DEBUG;
	}else if(check_for_parameter("--spam", argc_, argv_)){
		print_level = PRINTF_SPAM;
	}else{
		print_level = PRINTF_UNLIKELY_WARN;
	}
	if(check_for_parameter("--abort-error", argc_, argv_)){
		abort_print_level = PRINTF_ERROR;
	}else if(check_for_parameter("--abort-unlikely-warn", argc_, argv_)){
		abort_print_level = PRINTF_UNLIKELY_WARN;
	}else if(check_for_parameter("--abort-likely-warn", argc_, argv_)){
		abort_print_level = PRINTF_LIKELY_WARN;
	}else{
		abort_print_level = 0; // PRINTF_VITAL should abort anyways
	}
}

void misc_init(){
	std::set_new_handler(free_ram);
	test_random();
	test_ids();
	set_print_levels();
}

void throw_if_nullptr(void* a){
	if(a == nullptr){
		throw std::logic_error("nullptr");
	}
}
