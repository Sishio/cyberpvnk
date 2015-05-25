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

coord_t::coord_t(bool add) : array(this, add){
	array.data_lock.lock();
	array.long_double_lock.lock();
	array.long_double_array.push_back(&x);
	array.long_double_array.push_back(&y);
	array.long_double_array.push_back(&z);
	array.long_double_array.push_back(&x_angle);
	array.long_double_array.push_back(&y_angle);
	array.long_double_array.push_back(&x_vel);
	array.long_double_array.push_back(&y_vel);
	array.long_double_array.push_back(&z_vel);
	array.long_double_lock.unlock();
	array.int_lock.lock();
	array.int_array.push_back(&model_id);
	array.int_lock.unlock();
	array.data_lock.unlock();
	array.reset_values();
	array.data_lock.lock();
	array.data_type = "coord_t";
	old_time = get_time();
	mobile = true;
	array.data_lock.unlock();
}

void coord_t::print(){
	array.data_lock.lock();
	printf("X:%Lf\tY:%Lf\tZ:%Lf\n",x,y,z);
	printf("Vel_x:%Lf\tVel_Y:%Lf\tVel_z:%Lf\n",x_vel,y_vel,z_vel);
	printf("X_angle:%Lf\tY_angle:%Lf\n",x_angle,y_angle);
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

coord_t::~coord_t(){
	delete (model_t*)find_pointer(model_id, "model_t");
}

model_t::model_t(bool add) : array(this, add){
	array.data_type = "model_t";
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

client_t::client_t(bool add) : array(this, add){
	array.int_array.push_back(&model_id);
	array.int_array.push_back(&coord_id);
	array.int_array.push_back(&connection_info_id);
	array.data_type = "client_t";
}

client_t::~client_t(){
	delete (coord_t*)find_pointer(coord_id, "coord_t");
	delete (model_t*)find_pointer(model_id, "model_t");
	delete (net_ip_connection_info_t*)find_pointer(connection_info_id, "net_ip_connection_info_t");
}

/*render_buffer_t::render_buffer_t(bool add) : array(this, add){
	array.int_array.push_back(&coord_id);
	array.int_array.push_back(&model_id);
	array.data_type = "render_buffer_t";
}

render_buffer_t::~render_buffer_t(){}
*/

net_ip_connection_info_t::net_ip_connection_info_t(bool add) : array(this, add){
	array.data_type = "net_ip_connection_info_t";
	array.int_array.push_back(&port);
	array.int_array.push_back(&connection_type);
	array.string_array.push_back(&ip);
}

net_ip_connection_info_t::~net_ip_connection_info_t(){}

input_settings_t::input_settings_t(bool add) : array(this, add){
	for(int_ i = 0;i < 64;i++){
		int_data[i][0] = int_data[i][1] = -1;
		array.int_array.push_back(&int_data[i][0]);
		array.int_array.push_back(&int_data[i][1]);
	}
}

input_settings_t::~input_settings_t(){}

void* class_new(std::string type){
	if(type == "client_t"){
		return new client_t;
	}else if(type == "coord_t"){
		return new coord_t;
	}else if(type == "model_t"){
		return new model_t;
	}else if(type == "net_ip_connection_info_t"){
		return new net_ip_connection_info_t;
	}else if(type == "input_settings_t"){
		return new input_settings_t;
	}else{
		printf("TODO: Make an allocator for '%s'\n", type.c_str());
		return nullptr;
	}
}
