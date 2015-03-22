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
#include "c_input_engine.h"

extern array_id_t host_info_id;
extern render_t *render;
extern net_t *net;
input_t *input = nullptr;

extern int argc_;
extern char **argv_;

extern net_t *net;
extern input_t *input;

void input_init(){
	input = new input_t(argc_, argv_);
	//input->render = render;
	loop_add(&loop, "input_engine", input_engine);
}

void input_engine(){
	input->loop();
}

void input_close(){
	delete input;
	input = nullptr;
	loop_del(&loop, input_engine);
}
