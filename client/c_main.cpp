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
#include "c_main.h"
#include "c_net_engine.h"
#include "c_input_engine.h"
#include "c_render_engine.h"

std::vector<coord_t*> coord;
std::vector<coord_extra_t> coord_extra;
std::vector<model_t*> model;
std::vector<model_extra_t> model_extra;
std::vector<client_t*> client;

int self_id = -1;

// safeguards for when some engines are missing

extern net_t *net;
extern input_t *input;
extern render_t *render;

int argc_;
char **argv_;

unsigned long int tick = 0;
bool terminate = false;
bool once_per_second = false;

static int choice = -1;
bool engine_and_module[3] = {true};

void test_logic_loop(){
	coord_t a;
	a.x = 1;
	a.y = 2;
	a.z = 3;
	a.x_angle = 4;
	a.y_angle = 5;
	a.x_vel = 6;
	a.y_vel = 7;
	a.z_vel = 8;
	coord_t b;
	int updated_int = INT_MAX;
	SET_BIT(&updated_int, ARRAY_INT_HASH_BIT, 1);
	SET_BIT(&updated_int, ARRAY_STRING_HASH_BIT, 1);
	SET_BIT(&updated_int, ARRAY_LONG_DOUBLE_HASH_BIT, 1);
	std::string the_string = a.array.gen_updated_string(updated_int);
	b.array.parse_string_entry(the_string);
	b.print();
	printf("\n\nfrom\n\n%s\n", the_string.c_str());
}

static void test_logic_init(){
	net_ip_connection_info_t *tmp_conn_info = new_net_ip_connection_info();
	tmp_conn_info->ip = "127.0.0.1";
	tmp_conn_info->port = NET_CLIENT_PORT;
	find_client_pointer(self_id)->connection_info_id = tmp_conn_info->array.id;
	net = new net_t(argc_, argv_, tmp_conn_info->array.id);
}

static void test_logic_close(){
	if(net != nullptr){
		delete net;
		net = nullptr;
	}else{
		printf("The network module has already been deleted. There is something wrong\n");
	}
}

static void render_all_init(){
	if(engine_and_module[0]){
		printf("Initializing the render engine\n");
		render_init();
	}

}

static void input_all_init(){
	if(engine_and_module[1]){
		printf("Initializing the input engine\n");
		input_init();
	}
}

static void net_all_init(){
	if(engine_and_module[2]){
		printf("Initializing the net engine\n");
		net_init();
	}

}

static void init(int choice){
	printf("Allocating & initializing self\n");
	self_id = new_client()->array.id;
	engine_and_module[0] = !check_for_parameter("--render-disable", argc_, argv_);
	engine_and_module[1] = !check_for_parameter("--input-disable", argc_, argv_);
	engine_and_module[2] = !check_for_parameter("--net-disable", argc_, argv_);
	switch(choice){
	case 1:
		render_all_init();
		net_all_init();
		input_all_init();
		break;
	case 2:
		test_logic_init();
		break;
	case 3:
		exit(0);
		break;
	
	}
}

static void render_all_close(){
	if(engine_and_module[0]){
		render_close();
	}
}

static void input_all_close(){
	if(engine_and_module[1]){
		input_close();
	}

}

static void net_all_close(){
	if(engine_and_module[2]){
		net_close();
	}
}

static void close(){
	switch(choice){
	case 1:
		render_all_close();
		input_all_close();
		net_all_close();
		break;
	case 2:
		test_logic_close();
		break;
	default:
		assert(false);
		break;
	}
}

static void function_timer(void (*function)(void), std::string function_title){
	long double start_time = get_time();
	function();
	printf("Time to execute '%s' function: %Lf\n",function_title.c_str(),(long double)(get_time()-start_time));
}

static void engine_loop(){
	if(choice == 1){
		function_timer(net_engine, "net_engine");
		function_timer(input_engine, "input_engine");
		function_timer(render_engine, "render_engine");
	}else{
		test_logic_loop();
	}
}

static void module_loop(){
	if(input != nullptr && input->loop() == TERMINATE) terminate = true;
	if(render != nullptr){
		client_t *self_tmp = find_client_pointer(self_id);
		if(self_tmp != nullptr){
			coord_t *coord_tmp = find_coord_pointer(self_tmp->coord_id);
			render->loop(coord_tmp);
		}
	}
}

static void loop(){
	engine_loop();
	module_loop();
}

static long double old_time = 0;
static long double once_per_second_time = 0;

static void once_per_second_check(){
	const long double curr_time = get_time();
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
	#ifdef __linux
	printf("Setting up the signal handler\n");
	signal(SIGINT,signal_handler);
	#endif
	choice = menu_loop();
	init(choice);
	while(terminate == false){
		loop();
		once_per_second_check();
		tick++;
	}
	close();
	return 0;
}
