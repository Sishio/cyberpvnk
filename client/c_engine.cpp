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

extern net_t *net;
extern render_t *render;
extern input_t *input;
extern client_t *self;
extern std::vector<coord_t*> coord;
extern std::vector<coord_extra_t> coord_extra;
extern std::vector<model_t*> model;
extern std::vector<model_extra_t> model_extra;

static coord_t *old_coord = nullptr;
static net_ip_connection_info_t server_connection_info;

static void net_engine_parse(std::string a){
	switch(a[0]){
	case ARRAY_HEADER_COORD_T:
		if(true){
			coord_t b;
			b.array->parse_string(a);
			store_coord_t(b, &coord, &coord_extra);
		}
		break;
	case ARRAY_HEADER_MODEL_T:
		if(true){
			model_t b;
			b.array->parse_string(a);
			store_model_t(b, &model, &model_extra);
		}
	default:
		break;
	}
}

void net_engine(){
	server_connection_info.ip = "127.0.0.1";
	server_connection_info.port = NET_IP_SERVER_RECEIVE_PORT;
	std::string a;
	while((a = net->read()) != ""){
		net_engine_parse(a);
	}
	if(unlikely(old_coord == nullptr)){
		old_coord = new coord_t;
	}
	if(likely(memcmp(&old_coord,self->coord,sizeof(coord_t)) != 0)){
		net->write(self->array->gen_string(), server_connection_info);
		printf("Wrote new coord data into the buffer\n");
	}
	memcpy(&old_coord,self->coord,sizeof(coord_t)); //padding
}

void input_engine(){
	input_buffer_t** input_buffer = input->input_buffer;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		if(input_buffer[i] != nullptr){
			if(input_buffer[i]->type == INPUT_TYPE_MOUSE_MOTION){ // In order to have a mouse_motion, the screen has to be initialized and thus the renderer has to be initia$
				float x_angle_add = (float)(input_buffer[i]->int_data[INPUT_TYPE_MOUSE_MOTION_X]-(int)((render->rules.x_res/2)));
				float y_angle_add = (float)(input_buffer[i]->int_data[INPUT_TYPE_MOUSE_MOTION_Y]-(int)((render->rules.y_res/2)));
				if(self->coord != nullptr){
					self->coord->set_x_angle(true,y_angle_add);
					self->coord->set_y_angle(true,x_angle_add);
				}
				delete[] input_buffer[i]->int_data;
				input_buffer[i]->int_data = nullptr;
				delete input_buffer[i];
				input_buffer[i] = nullptr;
				SDL_WarpMouseInWindow(render->render_screen,(render->rules.x_res/2),(render->rules.y_res/2));
			}else if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
				if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_KEY] == SDLK_f){
					ms_sleep(5000);
				}
			}
		}
	}
	//input_buffer = nullptr;
}

void render_engine(){
}
