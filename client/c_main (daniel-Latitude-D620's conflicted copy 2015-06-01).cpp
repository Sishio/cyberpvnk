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

loop_t init_, end_;

static void load_engines(){
	loop.name = "client loop code";
	init_.name = "init code";
	end_.name = "close code";
	init_.settings = 0;
	end_.settings = 0;
	if(check_for_parameter("--fast-init", argc_, argv_)){
		init_.settings |= LOOP_CODE_PARTIAL_MT;
	}
	switch(choice){
	case 1:
		if(!check_for_parameter("--render-disable", argc_, argv_)){
			loop_add(&init_, "render_init", render_init);
			loop_add(&end_, "render_close", render_close);
		}
		if(!check_for_parameter("--input-disable", argc_, argv_)){
			loop_add(&init_, "input_init", input_init);
			loop_add(&end_, "input_close", input_close);
		}
		if(!check_for_parameter("--net-disable", argc_, argv_)){
			loop_add(&init_, "net_init", net_init);
			loop_add(&end_, "net_close", net_close);
		}
		break;
	case 2:
		loop_add(&init_, "test_logic_init", test_logic_init);
		loop_add(&end_, "test_logic_close", test_logic_close);
		break;
	case 3:
		exit(0);
		break;
	default:
		assert(false);
		break;
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
	choice = menu_loop();
	load_engines();
	loop.settings |= LOOP_CODE_NEVEREND_MT;
	if(check_for_parameter("--partial-mt", argc_, argv_)){
		loop.settings |= LOOP_CODE_PARTIAL_MT;
	}else if(check_for_parameter("--no-mt", argc_, argv_)){
		loop.settings &= ~LOOP_CODE_PARTIAL_MT & ~LOOP_CODE_NEVEREND_MT;
	}
	loop_run(&init_);
	printf_("STATUS: Stopped init loop. Starting main loop\n", PRINTF_STATUS);
	while(likely(check_signal(SIGINT) == false && check_signal(SIGKILL) == false && check_signal(SIGTERM) == false)){
		loop_run(&loop);
		once_per_second_update();
	}
	printf_("STATUS: Stopped main loop. Starting close loop\n", PRINTF_STATUS);
	loop_run(&end_);
	close();
	return 0;
}
