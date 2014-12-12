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
extern int argc_;
extern char** argv_;

static coord_t *old_coord = nullptr;
static net_ip_connection_info_t server_connection_info;

static void net_engine_parse(std::string a){
	update_data(a);
}

static void net_write_array_vector(std::vector<std::vector<std::string>> a, net_ip_connection_info_t b){
	const unsigned int a_size = a.size();
	for(unsigned int i = 0;i < a_size;i++){
		const unsigned int a_i_size = a[i].size();
		for(unsigned int n = 0;n < a_i_size;n++){
			net->write(a[i][n],b);
		}
	}
}

void net_init(){
	for(int i = 0;i < argc_;i++){
		if(strcmp(argv_[i], (char*)"--net-ip-server-ip") == 0){
			server_connection_info.ip = (std::string)argv_[i+1];
		}else if(strcmp(argv_[i], (char*)"--net-ip-server-port") == 0){
			server_connection_info.port = atoi((char *)argv_[i+1]);
		}
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
		std::vector<std::vector<std::string>> a = self->array->gen_string_vector();
		net_write_array_vector(a, server_connection_info);
	}
	memcpy(&old_coord,self->coord,sizeof(coord_t)); //padding
}

void input_init(){
}

void input_engine(){
	input_buffer_t** input_buffer = input->input_buffer;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		if(input_buffer[i] != nullptr){
			if(input_buffer[i]->type == INPUT_TYPE_MOUSE_MOTION){
				SDL_WarpMouseInWindow(render->render_screen,(render->rules.x_res/2),(render->rules.y_res/2));
			}else if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
				if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_KEY] == SDLK_f){
					ms_sleep(5000);
				}
			}
		}
		if(likely(input_buffer[i] != nullptr)){
			std::vector<std::vector<std::string>> a = input_buffer[i]->array->gen_string_vector();
			net_write_array_vector(a, server_connection_info);
		}
	}
}

void render_init(){
}

void render_engine(){
}
