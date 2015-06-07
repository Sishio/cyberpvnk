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
#include "../loop/loop_main.h"
#include "../main.h"
#include "../net/net_main.h"
#include "../input/input_main.h"
#include "../util/util_main.h"
#include "../class/class_main.h"
#include "../math/math_main.h"
#include "../console/console_main.h"
#include "c_test_logic.h"
#include "c_net_engine.h"
#include "c_input_engine.h"
#include "c_render_engine.h"
#include "c_main.h"


array_id_t self_id = 0;
extern net_t *net;
extern input_t *input;
int_ argc_;
char **argv_;
loop_t *loop = nullptr;
static int_ choice;

loop_t *init_ = nullptr, *end_ = nullptr;

static void load_engines(){
	misc_init(); // takes precedence over everything else
	loop = new loop_t;
	init_ = new loop_t;
	end_ = new loop_t;
	loop->array.name = "client loop code";
	loop->settings = LOOP_CODE_PARTIAL_MT;
	init_->array.name = "init code";
	end_->array.name = "close code";
	init_->settings = end_->settings = 0;
	int_ init_loop_settings = 0, end_loop_settings = 0;
	if(check_for_parameter("--fast-init", argc_, argv_)){
		init_loop_settings = LOOP_CODE_PARTIAL_MT;
	}
	if(check_for_parameter("--fast-close", argc_, argv_)){ // who needs this?
		end_loop_settings = LOOP_CODE_PARTIAL_MT;
	}
	loop_add(init_, loop_generate_entry(init_loop_settings, "console_init", console_init));
	switch(choice){
	case 1:
		if(!check_for_parameter("--render-disable", argc_, argv_)){
			loop_add(init_, loop_generate_entry(init_loop_settings, "render_init", render_init));
			loop_add(end_, loop_generate_entry(end_loop_settings, "render_close", render_close));
		}
		if(!check_for_parameter("--input-disable", argc_, argv_)){
			loop_add(init_, loop_generate_entry(init_loop_settings, "input_init", input_init));
			loop_add(end_, loop_generate_entry(end_loop_settings, "input_close", input_close));
		}
		if(!check_for_parameter("--net-disable", argc_, argv_)){
			loop_add(init_, loop_generate_entry(init_loop_settings, "net_init", net_init));
			loop_add(end_, loop_generate_entry(end_loop_settings, "net_close", net_close));
		}
		break;
	case 2:
		loop_add(init_, loop_generate_entry(init_loop_settings, "test_logic_init", test_logic_init));
		loop_add(end_, loop_generate_entry(end_loop_settings, "test_logic_close", test_logic_close));
		break;
	case 3:
		exit(0);
		break;
	case 4:
		break;
	default:
		assert(false);
		break;
	}
	loop_add(end_, loop_generate_entry(end_loop_settings, "console_close", console_close));
	// console should always be the last thing to open for debugging purposes
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	choice = menu_loop();
	load_engines();
	loop_run(init_);
	printf_("STATUS: Stopped init loop. Starting main loop\n", PRINTF_STATUS);
	while(likely(check_signal(SIGINT) == false && check_signal(SIGKILL) == false && check_signal(SIGTERM) == false)){
		loop_run(loop);
		once_per_second_update();
	}
	printf_("STATUS: Stopped main loop. Starting close loop\n", PRINTF_STATUS);
	loop_run(end_);
	//delete_all_data();
	// this makes the program hang, so let's not do this here
	return 0;
}
