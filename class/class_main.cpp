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
#include "limits.h"
#include "class_main.h"

void coord_t::update_array_pointers(){
	array.update_pointers(); // clears everything
	array.long_double_array.push_back(&x);
	array.long_double_array.push_back(&y);
	array.long_double_array.push_back(&z);
	array.long_double_array.push_back(&x_angle);
	array.long_double_array.push_back(&y_angle);
	array.long_double_array.push_back(&x_vel);
	array.long_double_array.push_back(&y_vel);
	array.long_double_array.push_back(&z_vel);
	array.int_array.push_back(&model_id);
	array.data_type = "coord_t";
}

coord_t::coord_t(bool add) : array(this, add){
	x = y = z = x_angle = y_angle = x_vel = y_vel = z_vel = 0;
	physics_time = 0;
	old_time = get_time();
	model_id = -1;
	mobile = true;
	update_array_pointers();
}

void coord_t::print(){
	printf("X:%Lf\tY:%Lf\tZ:%Lf\n",x,y,z);
	printf("Vel_x:%Lf\tVel_Y:%Lf\tVel_z:%Lf\n",x_vel,y_vel,z_vel);
	printf("X_angle:%Lf\tY_angle:%Lf\n",x_angle,y_angle);
}

void coord_t::set_x_angle(bool add, long double a){
	if(add){
		x_angle += a;
		while(x_angle > 360) x_angle -= 360;
		while(x_angle < 0) x_angle += 360;
	}else x_angle = a;
}

void coord_t::set_y_angle(bool add, long double a){
	if(add){
		y_angle += a;
		while(y_angle > 360) y_angle -= 360;
		while(y_angle < 0) y_angle += 360;
	}else y_angle = a;
}

coord_t::~coord_t(){}

void model_t::update_array(){
	array.data_type = "model_t";
}

model_t::model_t(bool add) : array(this, add){
	update_array();
}

model_t::~model_t(){
	const unsigned long int coord_size = coord.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		delete coord[i];
		coord[i] = nullptr;
	}
	coord.clear();

	const unsigned long int vertex_size = vertex.size();
	for(unsigned long int i = 0;i < vertex_size;i++){
		delete vertex[i];
		vertex[i] = nullptr;
	}
	vertex.clear();

	const unsigned long int faces_size = faces.size();
	for(unsigned long int i = 0;i < faces_size;i++){
		delete faces[i];
		faces[i] = nullptr;
	}
	faces.clear();

	const unsigned long int normals_size = normals.size();
	for(unsigned long int i = 0;i < normals_size;i++){
		delete normals[i];
		normals[i] = nullptr;
	}
	normals.clear();

	texture.clear();
	lists.clear();

	const unsigned long int materials_size = materials.size();
	for(unsigned long int i = 0;i < materials_size;i++){
		delete materials[i];
		materials[i] = nullptr;
	}
	materials.clear();

	const unsigned long int texturecoordinate_size = texturecoordinate.size();
	for(unsigned long int i = 0;i < texturecoordinate_size;i++){
		delete texturecoordinate[i];
		texturecoordinate[i] = nullptr;
	}
	texturecoordinate.clear();
	ismaterial = isnormals = istexture = false;
}

void model_t::get_size(long double *x, long double *y, long double *z){
	x[0] = x[1] = 0;
	y[0] = y[1] = 0;
	z[0] = z[1] = 0;
}

void client_t::update_array(){
	array.update_pointers();
	array.int_array.push_back(&model_id);
	array.int_array.push_back(&coord_id);
	array.int_array.push_back(&connection_info_id);
	array.data_type = "client_t";
}

client_t::client_t(bool add) : array(this, add){
	update_array();
}

client_t::~client_t(){}

render_buffer_t::render_buffer_t(bool add) : array(this, add){
	array.int_array.push_back(&coord_id);
	array.int_array.push_back(&model_id);
	array.data_type = "render_buffer_t";
}

render_buffer_t::~render_buffer_t(){}

input_buffer_t::input_buffer_t(bool add) : array(this, add){
	array.id = 0; // reserve zero for input_buffers because ID collisions could happen and these aren't referred to outside of the main input function in the server.
	array.data_type = "input_buffer_t";
	array.int_array.push_back(&type);
	array.int_array.push_back(&client_id);
	for(unsigned int i = 0;i < 8;i++){
		array.int_array.push_back(&int_data[i]);
	}
	type = 0;
	for(unsigned int i = 0;i < 8;i++){
		int_data[i] = 0;
	}
	//TODO: make the array bigger and split it in half. one half has to be filled out verbatium for the key to match and the other doesn't have to be (perhaps a timestamp or something like that).
}

input_buffer_t::~input_buffer_t(){}

net_ip_connection_info_t::net_ip_connection_info_t(bool add) : array(this, add){
	array.data_type = "net_ip_connection_info_t";
	array.int_array.push_back(&port);
	array.int_array.push_back(&connection_type);
	array.string_array.push_back(&ip);
}

net_ip_connection_info_t::~net_ip_connection_info_t(){}

input_settings_t::input_settings_t(bool add) : array(this, add){
	for(unsigned long int i = 0;i < 64;i++){
		int_data[i][0] = int_data[i][1] = -1;
		array.int_array.push_back(&int_data[i][0]);
		array.int_array.push_back(&int_data[i][1]);
	}
}

input_settings_t::~input_settings_t(){}

gametype_t::gametype_t(std::string gametype_file_, bool add) : array(this, add){
	if(gametype_file_ == ""){
		printf("assuming this is a client and the server data has not been loaded yet\n");
	}else{
		gametype_file = gametype_file_;
		reload_gametype();
	}
	array.int_array.push_back(&base_gametype);
	array.int_array.push_back(&win);
	array.int_array.push_back(&win_degree);
	array.int_array.push_back(&point);
	array.int_array.push_back(&point_degree);
	array.int_array.push_back(&team_max_count);
	array.int_array.push_back(&team_max_size);
	array.int_array.push_back(&round_max_count);
	array.int_array.push_back(&round_max_size);
	array.data_type = "gametype_t";
}

void gametype_t::apply_core_gametype(){
	switch(base_gametype){
	case GAMETYPE_FFA:
		win = GAMETYPE_WIN_POINT_COUNT;
		win_degree = 50;
		point = GAMETYPE_POINT_KILL;
		point_degree = 1;
		team_max_count = 512;
		team_max_size = 1;
		round_max_count = 32;
		round_max_size = 60*60;
		break;
	case GAMETYPE_TDM:
		win = GAMETYPE_WIN_POINT_COUNT;
		win_degree = 50;
		point = GAMETYPE_POINT_KILL;
		point_degree = 1;
		team_max_count = 4;
		team_max_size = 128;
		round_max_count = 32;
		round_max_size = 60*60;
		break;
	case GAMETYPE_FREE:
		win = GAMETYPE_WIN_POINT_COUNT;
		win_degree = 50;
		point = GAMETYPE_POINT_KILL;
		point_degree = 1;
		team_max_count = 512;
		team_max_size = 1;
		round_max_count = 32;
		round_max_size = 60*6000;
		break;
	}
}

void gametype_t::reload_gametype(){
	if(gametype_file != ""){
		std::ifstream in(gametype_file);
		if(in.is_open()){
			char line[512];
			while(in.getline(line, 512)){
				std::string tmp_gametype_array[8];
				std::stringstream ss;
				ss << line;
				ss >> tmp_gametype_array[0] >>
					tmp_gametype_array[1] >> 
					tmp_gametype_array[2] >> 
					tmp_gametype_array[3] >> 
					tmp_gametype_array[4] >> 
					tmp_gametype_array[5] >> 
					tmp_gametype_array[6] >> 
					tmp_gametype_array[7];
				if(tmp_gametype_array[0] == "gametype"){
					if(tmp_gametype_array[1] == "ffa"){
						base_gametype = GAMETYPE_FFA;
					}else if(tmp_gametype_array[2] == "tdm"){
						base_gametype = GAMETYPE_TDM;
					}else if(tmp_gametype_array[3] == "free"){
						base_gametype = GAMETYPE_FREE;
					}
					apply_core_gametype();
				}
			}
		}
	}
}

gametype_t::~gametype_t(){
}
