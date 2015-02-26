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

array_id_t self_id = 0;

extern net_t *net;
extern input_t *input;
extern render_t *render;

int argc_;
char **argv_;

unsigned long int tick = 0;
bool terminate = false;

loop_t loop;

static int choice;

//#define TEST_LOGIC_IP "127.0.0.1"

static void test_logic_loop(){
	coord_t *coord = new coord_t;
	model_t *model = new model_t;
	coord->model_id = model->array.id;
	model_load(model, "test.obj");
	client_t *tmp_client = (client_t*)find_pointer(self_id);
	render->loop((coord_t*)(find_pointer(tmp_client->coord_id)));
	delete model;
	delete coord;

/*	net_ip_connection_info_t tmp_conn_info;
	tmp_conn_info.ip = TEST_LOGIC_IP;
	tmp_conn_info.port = NET_CLIENT_PORT;
	std::string send_data = tmp_conn_info.array.gen_updated_string(INT_MAX);
	for(unsigned long int i = 0;i < 65536;i++){
		net->write(send_data, tmp_conn_info.array.id);
	}
	net->loop();
	std::string read_value = net->read();
	while((read_value = net->read()) != ""){
		net_ip_connection_info_t tmp;
		tmp.array.parse_string_entry(read_value);
		printf("tmp.ip: %s\ttmp.port: %d\n",tmp.ip.c_str(), tmp.port);
	}
	ms_sleep(1000);
*/
}

static net_ip_connection_info_t tmp_conn_info;

static void test_logic_init(){
	loop_add(&loop, "test_logic_loop", test_logic_loop);
	render = new render_t(argc_, argv_);
	client_t *tmp_client = new client_t;
	self_id = tmp_client->array.id;
	tmp_client->model_id = (new model_t)->array.id;
	tmp_client->coord_id = (new coord_t)->array.id;
	tmp_client->connection_info_id = (new net_ip_connection_info_t)->array.id; // not needed
}

static void test_logic_close(){
	loop_del(&loop, test_logic_loop);
	if(render != nullptr){
		delete render;
		render = nullptr;
	}
}

static void init(){
	loop.name = "client loop code";
	switch(choice){
	case 1:
		render_init();
		net_init();
		input_init();
		break;
	case 2:
		test_logic_init();
		break;
	case 3:
		exit(0);
	}
}

static void close(){
	switch(choice){
	case 1:
		render_close();
		input_close();
		net_close();
		break;
	case 2:
		test_logic_close();
		break;
	default:
		assert(false);
		break;
	}
	delete_all_data();
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	#ifdef __linux
	printf("Setting up the signal handler\n");
	signal(SIGINT, basic_signal_handler);
	#endif
	choice = menu_loop();
	init();
	int loop_settings;
	while(terminate == false){
		loop_run(&loop, &loop_settings); // 0: don't multithread it. 1: multithread all of it
		once_per_second_update();
		tick++;
	}
	close();
	return 0;
}
