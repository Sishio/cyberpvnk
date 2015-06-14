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
#include "../class/class_main.h"
#include "../class/class_array.h"
#include "../input/input_main.h"
#include "c_net_engine.h"
#include "c_render_engine.h"
#include "c_main.h"
#include "c_input_engine.h"

extern array_id_t host_info_id;
extern net_t *net;

void input_init(){
	loop_add(loop, loop_generate_entry(0, "input_engine", input_engine));
	input_t *tmp = new input_t();
	tmp->set_keyboard_map_id((new input_keyboard_map_t)->array.id);
}

void input_engine(){
	try{
		input_t *input = (input_t*)find_pointer(all_ids_of_type("input_t")[0]);
		throw_if_nullptr(input);
		input->array.data_lock.lock(); // no throws are allowed beyond this point
		client_t *self = (client_t*)find_pointer(self_id);
		if(self != nullptr && self->keyboard_map_id != DEFAULT_INT_VALUE){
			input->array.data_lock.unlock();
			input->set_keyboard_map_id(self->keyboard_map_id);
			input->array.data_lock.lock();
		}
		input->loop();
		if(input->query_key(SDL_SCANCODE_ESCAPE)){
			set_signal(SIGTERM, true);
		}
		input->array.data_lock.unlock();
	}catch(std::logic_error &e){
		printf_("WARNING: input_t: cannot use the input library yet\n", PRINTF_UNLIKELY_WARN);
	}
}

void input_close(){
	try{
		input_t *input = (input_t*)find_pointer(all_ids_of_type("input_t")[0]);
		throw_if_nullptr(input);
		delete input;
		input = nullptr;
	}catch(const std::logic_error &e){}
	loop_del(loop, input_engine);
}
