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
#include "../util/util_main.h"
#include "limits.h"
#include "class_main.h"

int_ coord_t::dimensions(){
	return dimensions_;
}

coord_t::coord_t() : array(this, "coord_t", ARRAY_SETTING_SEND){
	array.data_lock.lock();
	array.long_double_lock.lock();
	array.long_double_array.push_back(std::make_pair(&x, "x coordinate"));
	array.long_double_array.push_back(std::make_pair(&y, "y coordinate"));
	array.long_double_array.push_back(std::make_pair(&z, "z coordinate"));
	array.long_double_array.push_back(std::make_pair(&x_angle, "x angle"));
	array.long_double_array.push_back(std::make_pair(&y_angle, "y angle"));
	array.long_double_array.push_back(std::make_pair(&x_vel, "x velocity"));
	array.long_double_array.push_back(std::make_pair(&y_vel, "y velocity"));
	array.long_double_array.push_back(std::make_pair(&z_vel, "z velocity"));
	array.long_double_lock.unlock();
	array.int_lock.lock();
	array.int_array.push_back(std::make_pair(&model_id, "model id"));
	array.int_array.push_back(std::make_pair(&tile_id, "tile id"));
	array.int_array.push_back(std::make_pair(&dimensions_, "dimensions"));
	array.int_lock.unlock();
	array.data_lock.unlock();
	array.reset_values();
	array.data_lock.lock();
	old_time = get_time();
	dimensions_ = 2;
	model_id = tile_id = 0;
	array.data_lock.unlock();
}

void coord_t::set_x_angle(bool add, long double a){
	array.data_lock.lock();
	assert(std::abs(a) < 100);
	if(add){
		x_angle += a;
		while(x_angle > 360) x_angle -= 360;
		while(x_angle < 0) x_angle += 360;
	}else x_angle = a;
	array.data_lock.unlock();
}

void coord_t::set_y_angle(bool add, long double a){
	array.data_lock.lock();
	assert(std::abs(a) < 100);
	if(add){
		y_angle += a;
		while(y_angle > 360) y_angle -= 360;
		while(y_angle < 0) y_angle += 360;
	}else y_angle = a;
	array.data_lock.unlock();
}

coord_t::~coord_t(){}

model_t::model_t() : array(this, "model_t", ARRAY_SETTING_SEND){
	array.string_array.push_back(std::make_pair(&entire_object_file, "entire object file"));
}

model_t::~model_t(){
	const int_ coord_size = coord.size();
	for(int_ i = 0;i < coord_size;i++){
		delete coord[i];
		coord[i] = nullptr;
	}
	coord.clear();

	const int_ vertex_size = vertex.size();
	for(int_ i = 0;i < vertex_size;i++){
		delete vertex[i];
		vertex[i] = nullptr;
	}
	vertex.clear();

	const int_ faces_size = faces.size();
	for(int_ i = 0;i < faces_size;i++){
		delete faces[i];
		faces[i] = nullptr;
	}
	faces.clear();

	const int_ normals_size = normals.size();
	for(int_ i = 0;i < normals_size;i++){
		delete normals[i];
		normals[i] = nullptr;
	}
	normals.clear();

	texture.clear();
	lists.clear();

	const int_ materials_size = materials.size();
	for(int_ i = 0;i < materials_size;i++){
		delete materials[i];
		materials[i] = nullptr;
	}
	materials.clear();

	const int_ texturecoordinate_size = texturecoordinate.size();
	for(int_ i = 0;i < texturecoordinate_size;i++){
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

client_t::client_t(bool add) : array(this, "client_t", ARRAY_SETTING_SEND){
	array.int_array.push_back(std::make_pair(&model_id, "model ID"));
	model_id = DEFAULT_INT_VALUE;
	array.int_array.push_back(std::make_pair(&coord_id, "coord ID"));
	coord_id = DEFAULT_INT_VALUE;
	array.int_array.push_back(std::make_pair(&connection_info_id, "connection info ID"));
	connection_info_id = DEFAULT_INT_VALUE;
	array.int_array.push_back(std::make_pair(&keyboard_map_id, "keyboard map id"));
	keyboard_map_id = DEFAULT_INT_VALUE;
	array.data_type = "client_t";
}

client_t::~client_t(){}

net_ip_connection_info_t::net_ip_connection_info_t(bool add) : array(this, "net_ip_connection_info_t", ARRAY_SETTING_SEND){
	array.data_type = "net_ip_connection_info_t";
	array.int_array.push_back(std::make_pair(&port, "port"));
	array.int_array.push_back(std::make_pair(&connection_type, "connection type"));
	array.string_array.push_back(std::make_pair(&ip, "IP address"));
}

net_ip_connection_info_t::~net_ip_connection_info_t(){}

server_time_t::server_time_t() : array(this, "server_time_t", ARRAY_SETTING_SEND){
	array.int_array.push_back(std::make_pair(&current_unix_timestamp, "current_unix_timestamp"));
} // should always send out the current UNIX timestamp

void server_time_t::update_timestamp(){
	current_unix_timestamp = time(NULL); // hopefully this has been updated by 2037
}

int_ server_time_t::get_timestamp(){
	array.data_lock.lock();
	int_ return_value = current_unix_timestamp;
	array.data_lock.unlock();
	return return_value;
}

server_time_t::~server_time_t(){
	printf_("DEBUG: Closing server_time_t, hopefully the server is terminating\n", PRINTF_DEBUG);
}
