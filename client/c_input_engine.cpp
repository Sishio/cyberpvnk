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

#include "c_engine.h"
#include "c_input_engine.h"

extern int host_info_id;
extern int self_id;
extern input_t *input;
extern render_t *render;

static std::string package_input_buffer(input_buffer_t *a){
	std::string value;
	std::vector<std::vector<std::string>> return_value = a->array.gen_string_vector();
	for(unsigned long int i = 0;i < return_value.size();i++){
		for(unsigned long int n = 0;n < return_value[i].size();n++){
			value += return_value[i][n];
		}
	}
	return value;
}

void input_mouse_motion_engine(input_buffer_t *a){
	assert(render != nullptr);
	client_t *self_tmp = find_client_pointer(self_id);
	coord_t *coord = nullptr;
	if(self_tmp != nullptr){
		coord = find_coord_pointer(self_tmp->coord_id);
	}
	int x = a->int_data[INPUT_TYPE_MOUSE_MOTION_X];
	int y = a->int_data[INPUT_TYPE_MOUSE_MOTION_Y];
	const int screen_x_size_half = render->rules.x_res/2;
	const int screen_y_size_half = render->rules.y_res/2;
	if(coord != nullptr){
		coord->set_y_angle(true, x-screen_x_size_half);
		coord->set_x_angle(true, y-screen_y_size_half);
		if(net != nullptr){
			std::string data[2];
			data[0] = coord->array.generate_string_from_variable(&coord->x_angle);
			data[1] = coord->array.generate_string_from_variable(&coord->y_angle);
			net->write(data[0], host_info_id, gen_rand());
			net->write(data[1], host_info_id, gen_rand());
		}
		coord->print();
	}
	cursor::set_location(screen_x_size_half, screen_y_size_half);
	// don't send this input_buffer to them, just patch the current coord and send it to them
}

void input_mouse_press_engine(input_buffer_t *a){
	net->write(package_input_buffer(a), host_info_id, gen_rand());
}

void input_mouse_scroll_engine(input_buffer_t *a){
	net->write(package_input_buffer(a), host_info_id, gen_rand());
}

void input_keyboard_engine(input_buffer_t *a){
	if(a->int_data[INPUT_TYPE_KEYBOARD_KEY] == SDLK_f){
		ms_sleep(5000); // reserved key for debugging things
	}else{
		net->write(package_input_buffer(a), host_info_id, gen_rand());
	}
}

void input_init(){
}

void input_engine(){
	input->loop();
	for(unsigned int i = 0;i < input_buffer_vector.size();i++){
		while(input_buffer_vector[i] == nullptr) i++;
		input_buffer_vector[i]->client_id = self_id;
		switch(input_buffer_vector[i]->type){
		case INPUT_TYPE_MOUSE_MOTION:
			input_mouse_motion_engine(input_buffer_vector[i]);
			delete_input_buffer(input_buffer_vector[i]);
			break;
		default:
			std::vector<std::vector<std::string>> tmp_input = input_buffer_vector[i]->array.gen_string_vector();
			std::string send_data;
			for(unsigned long int n = 0;n < tmp_input.size();n++){
				for(unsigned long int c = 0;c < tmp_input[n].size();c++){
					send_data += tmp_input[n][c];
				}
			}
			net->write(send_data, host_info_id, gen_rand());
			break;
		}
	}
}

void input_close(){}
