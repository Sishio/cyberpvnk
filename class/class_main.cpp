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
#include "class_main.h"

extern void coord_reconstruct_vector(void*);
extern void model_reconstruct_vector(void*);

std::vector<coord_t*> coord_vector;
std::vector<model_t*> model_vector;
std::vector<client_t*> client_vector;

coord_t *new_coord();
model_t *new_model();

void coord_t::update_array_pointers(){
	array->long_double_array.clear();
	array->long_double_array.push_back(&x);
	array->long_double_array.push_back(&y);
	array->long_double_array.push_back(&z);
	array->long_double_array.push_back(&x_angle);
	array->long_double_array.push_back(&y_angle);
	array->long_double_array.push_back(&x_vel);
	array->long_double_array.push_back(&y_vel);
	array->long_double_array.push_back(&z_vel);
	array->int_array.clear();
	array->int_array.push_back(&model_id);

}

coord_t::coord_t(){
	x = y = z = x_angle = y_angle = x_vel = y_vel = z_vel = 0;
	physics_time = 0;
	old_time = get_time();
	model_id = -1;
	mobile = true;
	array = new_array();
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

void coord_t::close(){
	array->close();
	delete_array(array);
	array = nullptr;
}

void model_t::update_array_pointers(){

}

model_t::model_t(){
	array = new_array();
	update_array_pointers();
}

void model_t::get_size(long double *x, long double *y, long double *z){
	const int vertex_size = v.size();
	for(int i = 0;i < vertex_size;i++){
		if(x[0] > v[i][0]){
			x[0] = v[i][0];
		}else if(x[1] < v[i][0]){
			x[1] = v[i][0];
		}
		if(y[0] > v[i][0]){
			y[0] = v[i][0];
		}else if(y[1] < v[i][0]){
			y[1] = v[i][0];
		}
		if(z[0] > v[i][0]){
			z[0] = v[i][0];
		}else if(z[1] < v[i][0]){
			z[1] = v[i][0];
		}
	}
}

void model_t::load_parse_vector(std::string a){
	//std::string data[4];
	//std::stringstream ss;
	//ss << a;
	//ss >> data[0] >> data[1] >> data[2] >> data[3];
	//std::vector<double> b;
	//b.push_back(atof(data[1].c_str()));
	//b.push_back(atof(data[2].c_str()));
	//b.push_back(atof(data[3].c_str()));
	//b.push_back(b);
}

void model_t::load(std::string a){
	std::ifstream in(a);
	if(!in.is_open()){
		return;
	}
	char data_[512];
	while(in.getline(data_,511)){
		data_[511] = '\0';
		std::string data = data_;
		switch(data[0] + data[1]){
			case 'v'+' ':
				load_parse_vector(data);
				break;
			case 'v'+'t':
			case 'v'+'n':
			default:
				break;
		}
	}
}

void model_t::close(){
	array->close();
	delete_array(array);
	array = nullptr;
}

void client_t::update_array(){
	array->int_array.clear();
	array->int_array.push_back(&model_id);
	array->int_array.push_back(&coord_id);
}

client_t::client_t(){
	model_id = -1;
	coord_id = -1;
	model_t *model = find_model_pointer(model_id);
	coord_t *coord = find_coord_pointer(coord_id);
	printf("\tAllocating & initializing coord\n");
	coord = new_coord();
	printf("\tAllocating & initializing model\n");
	model = new_model();
	printf("\tAllocating & initializing array\n");
	array = new_array();
	update_array();
}

void client_t::close(){
	delete_array(array);
	array = nullptr;
}

coord_t *new_coord(){
	coord_t *return_value = new coord_t;
	return return_value;
}

model_t *new_model(){
	model_t *return_value = new model_t;
	return return_value;
}

void add_coord(coord_t *a){
	const unsigned long int coord_size = coord_vector.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		if(coord_vector[i] == nullptr){
			coord_vector[i] = a;
			return;
		}
	}
	coord_vector.push_back(a);
}

void add_model(model_t *a){
	const unsigned long int model_size = model_vector.size();
	for(unsigned long int i = 0;i < model_size;i++){
		if(model_vector[i] == nullptr){
			model_vector[i] = a;
			return;
		}
	}
	model_vector.push_back(a);
	
}

// change the id from int to long long int when I get around to making more data types in the array
coord_t *find_coord_pointer(int id){
	const unsigned long int coord_size = coord_vector.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		if(coord_vector[i]->array->id == id){
			return coord_vector[i];
		}
	}
	return nullptr;
	
}

model_t *find_model_pointer(int id){
	const unsigned long int model_size = model_vector.size();
	for(unsigned long int i = 0;i < model_size;i++){
		if(model_vector[i]->array->id == id){
			return model_vector[i];
		}
	}
	return nullptr;
	
}
