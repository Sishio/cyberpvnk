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

std::vector<coord_t*> coord;
std::vector<coord_extra_t> coord_extra;
std::vector<model_t*> model;
std::vector<model_extra_t> model_extra;
std::vector<client_t*> client;

int host_info_id;
int self_id = -1;

render_t *render = nullptr;
input_t *input = nullptr;
net_t *net = nullptr;

int argc_;
char **argv_;

unsigned long int tick = 0;
bool terminate = false;
bool once_per_second = false;

static int choice = -1;
bool engine_and_module[6] = {true};

static unsigned long int test_logic_size = 1;

void test_logic_loop(){
	find_net_ip_connection_info_pointer(find_client_pointer(self_id)->connection_info_id)->ip = "127.0.0.1";
	find_net_ip_connection_info_pointer(find_client_pointer(self_id)->connection_info_id)->port = NET_CLIENT_PORT;
	std::string segment_string = "1234567890";
	for(unsigned long int i = 0;i < test_logic_size;i++){
		segment_string += segment_string;
	}
	long double byte_size = 10*pow(2, test_logic_size);
	printf("Generated string of %Lf megabytes\n", byte_size*BYTE_TO_MEGABYTE);
	net->write(segment_string, find_client_pointer(self_id)->connection_info_id, gen_rand());
	for(unsigned long int i = 0;i < 16;i++){
		net->loop();
	}
	std::string receive = net->read();
	if(strcmp(receive.c_str(), segment_string.c_str()) == 0){
		printf("SUCCESS! Both of the strings are the exact same.\n");
	}else if(receive != ""){
		printf("FAIL! There is a mismatch.\n");
	}
	test_logic_size++;
}

static void test_logic_init(){
	net_ip_connection_info_t *tmp_conn_info = new_net_ip_connection_info();
	tmp_conn_info->ip = "127.0.0.1";
	tmp_conn_info->port = NET_CLIENT_PORT;
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
	bool *a = engine_and_module;
	if(a[0]){
		printf("Allocating & initializing render module\n");
		render = new render_t(argc_,argv_);
		if(a[3]){
			printf("Initializing the render engine\n");
			render_init();
		}
	}

}

static void input_all_init(){
	bool *a = engine_and_module;
	if(a[1]){
		printf("Allocating & initializing input module\n");
		input = new input_t(argc_,argv_);
		if(a[4]){
			printf("Initializing the input engine\n");
			input_init();
		}
	}
}

static void net_all_init(){
	bool *a = engine_and_module;
	if(a[2]){
		printf("Allocating & initializing net module\n");
		net_ip_connection_info_t *self_info_tmp = find_net_ip_connection_info_pointer(find_client_pointer(self_id)->connection_info_id);
		self_info_tmp->ip = "127.0.0.1";
		self_info_tmp->port = NET_CLIENT_PORT;
		net = new net_t(argc_, argv_, self_info_tmp->array.id);
		if(a[5]){
			printf("Initializing the net engine\n");
			net_init();
		}
	}

}

static void init(int choice){
	printf("Allocating & initializing self\n");
	self_id = new_client()->array.id;
	bool *a = engine_and_module;
	a[0] = !check_for_parameter("--render-disable", argc_, argv_);
	a[1] = !check_for_parameter("--input-disable", argc_, argv_);
	a[2] = !check_for_parameter("--net-disable", argc_, argv_);
	a[3] = !check_for_parameter("--net-logic-disable", argc_, argv_);
	a[4] = !check_for_parameter("--input-logic-disable", argc_, argv_);
	a[5] = !check_for_parameter("--render-logic-disable", argc_, argv_);
	switch(choice){
	case 1:
		render_all_init();
		net_all_init();
		input_all_init();
		break;
	case 2:
		test_logic_init();
		break;
	}
}

static void render_all_close(){
	bool *a = engine_and_module;
	if(a[0]){
		if(a[3]){
			render_close();
		}
		render->close();
		delete render;
		render = nullptr;
	}
}

static void input_all_close(){
	bool *a = engine_and_module;
	if(a[1]){
		if(a[4]){
			input_close();
		}
		input->close();
		delete input;
		input = nullptr;
	}

}

static void net_all_close(){
	bool *a = engine_and_module;
	if(a[3]){
		if(a[5]){
			net_close();
		}
		delete net;
		net = nullptr;
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
		if(net != nullptr) function_timer(net_engine, "net_engine");
		if(input != nullptr) function_timer(input_engine, "input_engine");
		if(render != nullptr) function_timer(render_engine, "render_engine");
	}else{
		test_logic_loop();
	}
}

static void module_loop(){
	// net_engine takes care of the module itself
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

static void load(){
	if(likely(find_client_pointer(self_id) != nullptr)){
		if(unlikely(find_model_pointer(find_client_pointer(self_id)->model_id) == nullptr)){
			model_load(find_model_pointer(find_client_pointer(self_id)->model_id), "../cube.obj");
		}
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
		load();
		loop();
		once_per_second_check();
		tick++;
	}
	close();
}
