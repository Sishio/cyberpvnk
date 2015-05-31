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
#include "c_test_logic.h"
#include "c_net_engine.h"
#include "c_input_engine.h"
#include "c_render_engine.h"

array_id_t self_id = 0;
extern net_t *net;
extern input_t *input;
int_ argc_;
char **argv_;
loop_t loop;
static int_ choice;
static net_ip_connection_info_t tmp_conn_info;

static void init(){
	loop.name = "client loop code";
	loop_t init_loop;
	switch(choice){
	case 1:
		if(!check_for_parameter("--render-disable", argc_, argv_)) loop_add(&init_loop, "render_init", render_init);
		if(!check_for_parameter("--input-disable", argc_, argv_)) loop_add(&init_loop, "input_init", input_init);
		if(!check_for_parameter("--net-disable", argc_, argv_)) loop_add(&init_loop, "net_init", net_init);
		break;
	case 2:
		test_logic_init();
		break;
	case 3:
		exit(0);
		break;
	default:
		assert(false);
		break;
	}
	if(check_for_parameter("--fast-init", argc_, argv_)) SET_BIT(init_loop.settings, LOOP_CODE_PARTIAL_MT, 1);
	loop_run(&init_loop);
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
	choice = menu_loop();
	init();
	SET_BIT(loop.settings, LOOP_CODE_NEVEREND_MT, 1);
	while(likely(check_signal(SIGINT) == false && check_signal(SIGKILL) == false && check_signal(SIGTERM) == false)){
		loop_run(&loop);
		once_per_second_update();
	}
	close();
	return 0;
}
