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
#define TEST_LOGIC 1
#include "c_main.h"
#include "signal.h"

std::vector<coord_t*> coord;
std::vector<coord_extra_t> coord_extra;
std::vector<model_t*> model;
std::vector<model_extra_t> model_extra;
std::vector<client_t*> client;

client_t *self = nullptr;

render_t *render = nullptr;
input_t *input = nullptr;
net_t *net = nullptr;

int argc_;
char **argv_;

unsigned long int tick = 0;
bool terminate = false;
bool once_per_second = false;

static bool check_for_parameter(const std::string a){
	for(int i = 0;i < argc_;i++){
		if(std::string(argv_[i]) == a){
			return true;
		}
	}
	return false;
}

static void init(){
	printf("Allocating & initializing self\n");
	self = new client_t();
	model_load(find_model_pointer(self->model_id), "../cube.obj");
	bool a[3] = {true};
	a[0] = !check_for_parameter("--render-disable");
	a[1] = !check_for_parameter("--input-disable");
	a[2] = !check_for_parameter("--net-disable");
	if(a[0]){
		printf("Allocating & initializing render module\n");
		render = new render_t(argc_,argv_);
		printf("Initializing the render engine\n");
		render_init();
	}
	if(a[1]){
		printf("Allocating & initializing input module\n");
		input = new input_t(argc_,argv_);
		printf("Initializing the input engine\n");
		input_init();
	}
	if(a[2]){
		printf("Allocating & initializing net module\n");
		net = new net_t(argc_,argv_);
		printf("Initializing the net engine\n");
		net_init();
	}
}

static void close(){
	if(render != nullptr){
		render_close();
		render->close();
		delete render;
		render = nullptr;
	}
	if(input != nullptr){
		input_close();
		input->close();
		delete input;
		input = nullptr;
	}
	if(net != nullptr){
		net_close();
		net->close();
		delete net;
		net = nullptr;
	}
	if(self != nullptr){
		self->close();
		delete self;
		self = nullptr;
	}
}

static void engine_loop(){
	if(net != nullptr) net_engine();
	if(input != nullptr) input_engine();
	if(render != nullptr) render_engine();
}

static void module_loop(){
	if(net != nullptr) net->loop();
	if(input != nullptr && input->loop() == TERMINATE) terminate = true;
	if(render != nullptr) render->loop(find_coord_pointer(self->coord_id));
}

static void loop(){
	engine_loop(); // logic code that uses the engine stuff
	module_loop(); // core engine code that the logic uses
}

#ifdef __linux
void signal_handler(int signal){
	if(signal == SIGINT){
		printf("Received SIGINT\n");
		terminate = true;
	}
}
#endif

static render_buffer_t *test_logic_render = nullptr;

void test_logic_function(){
	/*coord_t a,b;
	a.x = gen_rand();
	a.y = gen_rand();
	a.z = gen_rand();
	a.set_x_angle(false, gen_rand());
	a.set_y_angle(false, gen_rand());
	b.array->parse_string_vector(a.array->gen_string_vector(true));
	a.print();
	printf("--------------------------------------------------------------------\n");
	b.print();
	printf("\n\n\n");*/
	if(test_logic_render == nullptr){
		test_logic_render = new_render_buffer();
	}
	assert(test_logic_render != nullptr);
	test_logic_render->model_id = self->model_id;
	test_logic_render->coord_id = self->coord_id;
	find_coord_pointer(self->coord_id)->x = -100;
	find_coord_pointer(self->coord_id)->y = -100;
	//ms_sleep(1000);
}

void test_logic(){
	//long double time_start = get_time();
	test_logic_function();
	//long double time_end = get_time();
	//printf("Elapsed test_logic time: %Lf\n",time_end-time_start);
}

static long double old_time = 0;
static long double once_per_second_time = 0;

static void once_per_second_check(){
	long double curr_time = get_time();
	once_per_second_time += curr_time-old_time;
	old_time = curr_time;
	if(once_per_second_time >= 1){
		once_per_second_time -= 1;
		once_per_second = true;
	}else{
		once_per_second = false;
	}
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	printf("Initializing the modules and other data\n");
	init();
	#ifdef __linux
	printf("Setting up the signal handler\n");
	signal(SIGINT,signal_handler);
	// insert this into the input engine
	#endif
	while(terminate == false){
		loop();
		#ifdef TEST_LOGIC
		test_logic();
		#endif
		once_per_second_check();
		tick++;
	}
	close();
}
