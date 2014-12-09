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

coord_t::coord_t(){
	x = y = z = x_angle = y_angle = x_vel = y_vel = z_vel = 0;
	physics_time = 0;
	old_time = get_time();
	model_id = -1;
	mobile = true;
	array = new array_t(ARRAY_HEADER_COORD_T);
	array->long_double_array.push_back(&x);
	array->long_double_array.push_back(&y);
	array->long_double_array.push_back(&z);
	array->long_double_array.push_back(&x_angle);
	array->long_double_array.push_back(&y_angle);
	array->long_double_array.push_back(&x_vel);
	array->long_double_array.push_back(&y_vel);
	array->long_double_array.push_back(&z_vel);
	array->int_array.push_back(&model_id);
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
	delete array;
	array = nullptr;
}

model_t::model_t(){
	array = new array_t(ARRAY_HEADER_MODEL_T);
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
	std::string data[4];
	std::stringstream ss;
	ss << a;
	ss >> data[0] >> data[1] >> data[2] >> data[3];
	std::vector<double> b;
	b.push_back(atof(data[1].c_str()));
	b.push_back(atof(data[2].c_str()));
	b.push_back(atof(data[3].c_str()));
	v.push_back(b);
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
	delete array;
	array = nullptr;
}

client_t::client_t(){
	printf("\tAllocating & initializing coord\n");
	coord = new coord_t;
	printf("\tAllocating & initializing model\n");
	model = new model_t;
	printf("\tAllocating & initializing array\n");
	array = new array_t(ARRAY_HEADER_CLIENT_T);
	printf("\tLoading data into the array\n");
	for(unsigned long int i = 0;i < coord->array->int_array.size();i++) array->int_array.push_back(coord->array->int_array[i]);
	for(unsigned long int i = 0;i < coord->array->long_double_array.size();i++) array->long_double_array.push_back(coord->array->long_double_array[i]);
	for(unsigned long int i = 0;i < coord->array->string_array.size();i++) array->string_array.push_back(coord->array->string_array[i]);
        for(unsigned long int i = 0;i < model->array->int_array.size();i++) array->int_array.push_back(model->array->int_array[i]);
        for(unsigned long int i = 0;i < model->array->long_double_array.size();i++) array->long_double_array.push_back(model->array->long_double_array[i]);
        for(unsigned long int i = 0;i < model->array->string_array.size();i++) array->string_array.push_back(model->array->string_array[i]);
}

void client_t::close(){
	//coord->close();
	//model->close();
	delete coord;
	delete model;
	coord = nullptr;
	model = nullptr;
}
net_data_t::net_data_t(){
	ip = "";
	port = 0;
}
