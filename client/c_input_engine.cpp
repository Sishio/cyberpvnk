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

extern client_t *self;
extern input_t *input;
extern render_t *render;
extern net_ip_connection_info_t server_connection_info;

void input_mouse_motion_engine(input_buffer_t *a){
	assert(render != nullptr);
	assert(self != nullptr);
	coord_t *coord = find_coord_pointer(self->coord_id);
	int x = a->int_data[INPUT_TYPE_MOUSE_MOTION_X];
	int y = a->int_data[INPUT_TYPE_MOUSE_MOTION_Y];
	const int screen_x_size_half = render->rules.x_res/2;
	const int screen_y_size_half = render->rules.y_res/2;
	coord->set_y_angle(true, x-screen_x_size_half);
	coord->set_x_angle(true, y-screen_y_size_half);
	if(net != nullptr){
		std::string data[2];
		data[0] = coord->array->generate_string_from_variable(&coord->x_angle); // patch it
		data[1] = coord->array->generate_string_from_variable(&coord->y_angle);
		net->write(data[0], server_connection_info);
		net->write(data[1], server_connection_info);
	}
	coord->print();
	cursor::set_location(screen_x_size_half, screen_y_size_half);
	delete a;
	a = nullptr;
}

void input_mouse_press_engine(input_buffer_t *a){
	// package the data up and send it to the server
}

void input_mouse_scroll_engine(input_buffer_t *a){
	// package the data up and send it to the server
}

void input_keyboard_engine(input_buffer_t *a){
	if(a->int_data[INPUT_TYPE_KEYBOARD_KEY] == SDLK_f){
		ms_sleep(5000);
	}
}

void input_init(){
}

void input_engine(){
	input_buffer_t** input_buffer = input->input_buffer;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		if(input_buffer[i] == nullptr){
			goto input_loop_end;
		}
		switch(input_buffer[i]->type){
		case INPUT_TYPE_KEYBOARD:
			input_keyboard_engine(input_buffer[i]);
			break;
		case INPUT_TYPE_MOUSE_MOTION:
			input_mouse_motion_engine(input_buffer[i]);
			input_buffer[i] = nullptr;
			break;
		case INPUT_TYPE_MOUSE_PRESS:
			input_mouse_press_engine(input_buffer[i]);
			break;
		case INPUT_TYPE_MOUSE_SCROLL:
			input_mouse_scroll_engine(input_buffer[i]);
			break;
		default:
			break;
		}
		/*
		Don't delete anything from this array, the module will update itself
		 */
		input_loop_end:;
	}
}

void input_close(){}

