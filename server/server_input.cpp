#include "../loop/loop_main.h"
#include "../input/input_main.h"
#include "server_input.h"

extern loop_t *server_loop_code;
extern int_ argc_;
extern char** argv_;

input_t *input = nullptr;

void input_init(){
	loop_add(server_loop_code, loop_generate_entry(0, "input_engine", input_engine));
	input = new input_t(); // there is no keyboard_map needed
}

void input_close(){
	loop_del(server_loop_code, input_engine);
	delete input;
	input = nullptr;
}

void input_engine(){
	input->loop();
}
