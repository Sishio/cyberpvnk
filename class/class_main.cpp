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

std::vector<coord_t*> coord_vector;
std::vector<model_t*> model_vector;
std::vector<render_buffer_t*> render_buffer_vector;
std::vector<net_ip_connection_info_t*> net_ip_connection_info_vector;
std::vector<client_t*> client_vector;
std::vector<input_buffer_t*> input_buffer_vector;

coord_t *new_coord();
model_t *new_model();

void coord_t::update_array_pointers(){
	array.long_double_array.clear();
	array.long_double_array.push_back(&x);
	array.long_double_array.push_back(&y);
	array.long_double_array.push_back(&z);
	array.long_double_array.push_back(&x_angle);
	array.long_double_array.push_back(&y_angle);
	array.long_double_array.push_back(&x_vel);
	array.long_double_array.push_back(&y_vel);
	array.long_double_array.push_back(&z_vel);
	array.int_array.clear();
	array.int_array.push_back(&model_id);
	array.data_type = "coord_t";
}

coord_t::coord_t(){
	x = y = z = x_angle = y_angle = x_vel = y_vel = z_vel = 0;
	physics_time = 0;
	old_time = get_time();
	model_id = -1;
	mobile = true;
	add_array_to_vector(&array);
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

void coord_t::apply_motion(int type){
	const long double sine_y_rot = sin(y_angle*RADIAN);
	const long double cosine_y_rot = cos(y_angle*RADIAN);
	switch(type){
	case INPUT_MOTION_FORWARD:
		x_vel += sine_y_rot;
		z_vel += -cosine_y_rot;
		break;
	case INPUT_MOTION_BACKWARD:
		x_vel += -sine_y_rot;
		z_vel += cosine_y_rot;
		break;
	case INPUT_MOTION_LEFT:
		x_vel += -cosine_y_rot;
		z_vel += -sine_y_rot;
		break;
	case INPUT_MOTION_RIGHT:
		x_vel += cosine_y_rot;
		z_vel += sine_y_rot;
		break;
	default:
		assert(false);
		break;
	}
}

void coord_t::close(){
	array.close();
	delete_array_from_vector(&array);
}

void model_t::update_array(){
	array.data_type = "model_t";
}

model_t::model_t(){
	add_array_to_vector(&array);
	update_array();
}

void model_t::get_size(long double *x, long double *y, long double *z){
	x[0] = x[1] = 0;
	y[0] = y[1] = 0;
	z[0] = z[1] = 0;
}

void model_t::close(){
	array.close();
	delete_array_from_vector(&array);
	//delete all the dynamically allocated memory
	for(unsigned long int i=0;i<(unsigned long int)coord.size();i++)
		delete coord[i];
	for(unsigned long int i=0;i<(unsigned long int)faces.size();i++)
		delete faces[i];
	for(unsigned long int i=0;i<(unsigned long int)normals.size();i++)
		delete normals[i];
	for(unsigned long int i=0;i<(unsigned long int)vertex.size();i++)
		delete vertex[i];
	for(unsigned long int i=0;i<(unsigned long int)materials.size();i++)
		delete materials[i];
	for(unsigned long int i=0;i<(unsigned long int)texturecoordinate.size();i++)
		delete texturecoordinate[i];
	//and all elements from the vector
	coord.clear();
	faces.clear();
	normals.clear();
	vertex.clear();
	materials.clear();
	texturecoordinate.clear();

}

void client_t::update_array(){
	array.int_array.clear();
	array.int_array.push_back(&model_id);
	array.int_array.push_back(&coord_id);
	array.data_type = "client_t";
}

client_t::client_t(){
	coord_t *coord = new_coord();
	model_t *model = new_model();
	net_ip_connection_info_t *connection_info = new_net_ip_connection_info();
	add_array_to_vector(&array);
	model_id = model->array.id;
	coord_id = coord->array.id;
	connection_info_id = connection_info->array.id;
	find_coord_pointer(coord_id)->model_id = model_id;
	update_array();
}

void client_t::close(){
	delete_array_from_vector(&array);
}

render_buffer_t::render_buffer_t(){
	add_array_to_vector(&array);
	array.int_array.push_back(&coord_id);
	array.int_array.push_back(&model_id);
	array.data_type = "render_buffer_t";
}

client_t *new_client(){
	client_t *return_value = new client_t;
	client_vector.push_back(return_value);
	return return_value;
}

coord_t *new_coord(){
	coord_t *a = new coord_t;
	coord_vector.push_back(a);
	return a;
}

model_t *new_model(){
	model_t *a = new model_t;
	model_vector.push_back(a);
	return a;
}

input_buffer_t *new_input_buffer(){
	input_buffer_t *return_value = new input_buffer_t;
	input_buffer_vector.push_back(return_value);
	return return_value;
}

void delete_input_buffer_id(int id){
	const unsigned long int input_buffer_size = input_buffer_vector.size();
	for(unsigned long int i = 0;i < input_buffer_size;i++){
		if(input_buffer_vector[i]->array.id == id){
			delete input_buffer_vector[i];
			input_buffer_vector[i] = nullptr;
		}
	}
}

input_buffer_t::input_buffer_t(){
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
}

input_buffer_t::~input_buffer_t(){
	delete_array_from_vector(&array);
}

void delete_coord(int id){
	for(unsigned long int i = 0;i < coord_vector.size();i++){
		if(coord_vector[i]->array.id == id){
			delete coord_vector[i];
			coord_vector.erase(coord_vector.begin()+i);
			return;
		}
	}
}

coord_t *find_coord_pointer(int id){
	const unsigned long int coord_size = coord_vector.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		if(coord_vector[i]->array.id == id){
			return coord_vector[i];
		}
	}
	return nullptr;
}

model_t *find_model_pointer(int id){
	const unsigned long int model_size = model_vector.size();
	for(unsigned long int i = 0;i < model_size;i++){
		if(model_vector[i]->array.id == id){
			return model_vector[i];
		}
	}
	return nullptr;
}

render_buffer_t *new_render_buffer(){
	render_buffer_t *a = new render_buffer_t();
	render_buffer_vector.push_back(a);
	return a;
}

render_buffer_t *find_render_buffer_pointer(int id){
	const unsigned long int render_buffer_vector_size = render_buffer_vector.size();
	for(unsigned long int i = 0;i < render_buffer_vector_size;i++){
		if(render_buffer_vector[i]->array.id == id){
			return render_buffer_vector[i];
		}
	}
	return nullptr;
}

void add_render_buffer(render_buffer_t *a){
	const unsigned long int render_buffer_vector_size = render_buffer_vector.size();
	for(unsigned long int i = 0;i < render_buffer_vector_size;i++){
		if(render_buffer_vector[i] == nullptr){
			render_buffer_vector[i] = a;
			return;
		}
	}
	render_buffer_vector.push_back(a);
}

net_ip_connection_info_t* new_net_ip_connection_info(){
	net_ip_connection_info_t *return_value = new net_ip_connection_info_t;
	net_ip_connection_info_vector.push_back(return_value);
	return return_value;
}

net_ip_connection_info_t* find_net_ip_connection_info_pointer(int id){
	const unsigned long int net_ip_connection_info_size = net_ip_connection_info_vector.size();
	for(unsigned long int i = 0;i < net_ip_connection_info_size;i++){
		if(net_ip_connection_info_vector[i]->array.id == id){
			return net_ip_connection_info_vector[i];
		}
	}
	return nullptr;
}

client_t *find_client_pointer(int id){
	const unsigned long int client_size = client_vector.size();
	for(unsigned long int i = 0;i < client_size;i++){
		if(client_vector[i]->array.id == id){
			return client_vector[i];
		}
	}
	return nullptr;
}



void delete_input_buffer(input_buffer_t *a){
	if(unlikely(a == nullptr)) return;
	const unsigned long int input_buffer_size = input_buffer_vector.size();
	for(unsigned long int i = 0;i < input_buffer_size;i++){
		if(input_buffer_vector[i]->array.id == a->array.id){
			delete input_buffer_vector[i];
			input_buffer_vector[i] = nullptr;
			input_buffer_vector.erase(input_buffer_vector.begin()+i);
			return;
		}
	}
}

net_ip_connection_info_t::net_ip_connection_info_t(){
	array.data_type = "net_ip_connection_info_t";
	array.int_array.push_back(&port);
	array.int_array.push_back(&connection_type);
	array.string_array.push_back(&ip);
}

net_ip_connection_info_t::~net_ip_connection_info_t(){}


void delete_client_id(int a){
	for(unsigned long int i = 0;i < client_vector.size();i++){
		if(client_vector[i]->array.id == a){
			delete_array_id(client_vector[i]->array.id);
			delete client_vector[i];
			client_vector.erase(client_vector.begin()+i);
			return;
		}
	}
}

void delete_client_entry(int a){
	delete_array_id(client_vector[a]->array.id);
	delete client_vector[a];
	client_vector.erase(client_vector.begin()+a);
}

void delete_coord_entry(int a){
	delete_array_id(coord_vector[a]->array.id);
	delete coord_vector[a];
	coord_vector.erase(coord_vector.begin()+a);
}

input_settings_t::input_settings_t(){
	for(unsigned long int i = 0;i < 64;i++){
		array.int_array.push_back(&int_data[i][0]);
		array.int_array.push_back(&int_data[i][1]);
	}
}

input_settings_t::~input_settings_t(){}
