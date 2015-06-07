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
input_t *input = nullptr;

input_keyboard_map_t *local_keyboard_map = nullptr;
array_id_t current_keyboard_map_id;

void input_init(){
	loop_add(loop, loop_generate_entry(loop_entry_t(), "input_engine", input_engine));
	local_keyboard_map = new input_keyboard_map_t();
}

void input_engine(){
	client_t *client_ptr = (client_t*)find_pointer(self_id);
	if(input != nullptr && client_ptr != nullptr && current_keyboard_map_id == client_ptr->keyboard_map_id){ // use the client map
		delete input;
		input = nullptr; // just in case Valgrind throws a hissy fit
		input_keyboard_map_t *current_keyboard_map = (input_keyboard_map_t*)find_pointer(current_keyboard_map_id, "input_keyboard_map_t");
		if(current_keyboard_map != nullptr){
			input_keyboard_map_t *target_map = (input_keyboard_map_t*)find_pointer(((client_t*)client_ptr)->keyboard_map_id, "input_keyboard_map_t");
			memcpy(target_map->keyboard_map, current_keyboard_map->keyboard_map, sizeof(int_)*1024);
		} // copy all the keyboard data over so held keypresses don't get lost
		current_keyboard_map_id = ((client_t*)client_ptr)->keyboard_map_id;
		input = new input_t(argc_, argv_, current_keyboard_map_id); // just reset the module with the correct keyboard id
	}
	if(input == nullptr){
		if(client_ptr != nullptr){
			current_keyboard_map_id = ((client_t*)client_ptr)->keyboard_map_id;
			input = new input_t(argc_, argv_, current_keyboard_map_id);
		}else{
			current_keyboard_map_id = local_keyboard_map->array.id;
			input = new input_t(argc_, argv_, current_keyboard_map_id);
		}
	}
	input->loop();
	if(input->query_key(SDL_SCANCODE_ESCAPE)){
		set_signal(SIGTERM, true);
	}
}

void input_close(){
	delete input;
	input = nullptr;
	delete local_keyboard_map;
	local_keyboard_map = nullptr;
	loop_del(loop, input_engine);
}
