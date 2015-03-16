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

static void input_mouse_motion_engine(input_buffer_t *a){
	assert(render != nullptr);
	client_t *self_tmp = (client_t*)find_pointer(self_id, "client_t");
	coord_t *coord = nullptr;
	if(self_tmp != nullptr){
		coord = (coord_t*)find_pointer(self_tmp->coord_id, "coord_t");
	}
	int x = a->int_data[INPUT_TYPE_MOUSE_MOTION_X];
	int y = a->int_data[INPUT_TYPE_MOUSE_MOTION_Y];
	const int screen_x_size_half = render->rules->x_res/2;
	const int screen_y_size_half = render->rules->y_res/2;
	if(coord != nullptr){
		coord->set_y_angle(true, x-screen_x_size_half);
		coord->set_x_angle(true, y-screen_y_size_half);
		if(net != nullptr){
			int what_to_update = 0;
			coord->array.updated(&what_to_update); // TODO: Force the server to only use the x and y angles and NOT the rest of the coordinate
			net->write(coord->array.gen_updated_string(what_to_update), host_info_id);
		}
		coord->print();
	}
	//cursor::set_location(input, screen_x_size_half, screen_y_size_half);
}

void input_init(){
	input = new input_t(argc_, argv_);
	//input->render = render;
	loop_add(&loop, "input_engine", input_engine);
}

void input_engine(){
	input->loop();
	std::vector<void*> input_buffer = all_pointers_of_type("input_buffer_t");
	const unsigned long int input_buffer_size = input_buffer.size();
	for(unsigned int i = 0;i < input_buffer_size;i++){
		((input_buffer_t*)input_buffer[i])->client_id = self_id;
		switch(((input_buffer_t*)input_buffer[i])->type){
		case INPUT_TYPE_MOUSE_MOTION:
			input_mouse_motion_engine(((input_buffer_t*)input_buffer[i]));
			delete (input_buffer_t*)input_buffer[i];
			break;
		default:
			net->write(array_vector[i]->gen_updated_string(INT_MAX), host_info_id);
			break;
		}
	}
}

void input_close(){
	delete input;
	input = nullptr;
	loop_del(&loop, input_engine);
}
