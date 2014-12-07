#include "class_main.h"

void coord_t::init(){
	x = y = z = x_angle = y_angle = x_vel = y_vel = z_vel = 0;
	physics_time = 0;
	old_time = 0;
	model = nullptr;
	mobile = true;
	array = new array_t(ARRAY_HEADER_COORD_T);
	array->double_array.push_back(&x);
	array->double_array.push_back(&y);
	array->double_array.push_back(&z);
	array->double_array.push_back(&x_angle);
	array->double_array.push_back(&y_angle);
	array->double_array.push_back(&x_vel);
	array->double_array.push_back(&y_vel);
	array->double_array.push_back(&z_vel);
}

void coord_t::print(){
	printf("X:%f\tY:%f\tZ:%f\n",x,y,z);
	printf("Vel_x:%f\tVel_Y:%f\tVel_z:%f\n",x_vel,y_vel,z_vel);
	printf("X_angle:%f\tY_angle:%f\n",x_angle,y_angle);
}

void coord_t::set_x_angle(bool add, double a){
	if(add){
		x_angle += a;
		while(x_angle > 360) x_angle -= 360;
		while(x_angle < 0) x_angle += 360;
	}else x_angle = a;
}

void coord_t::set_y_angle(bool add, double a){
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

void model_t::init(){
	array = new array_t(ARRAY_HEADER_MODEL_T);
}

void model_t::get_size(double *x, double *y, double *z){
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
	v.push_back(b);
	v[v.size()-1].push_back(atof(data[1].c_str()));
	v[v.size()-1].push_back(atof(data[2].c_str()));
	v[v.size()-1].push_back(atof(data[3].c_str()));
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

void client_t::init(){
	coord = new coord_t;
	model = new model_t;
	coord->init();
	model->init();
	net.init();
	array = new array_t(ARRAY_HEADER_CLIENT_T);
	for(unsigned int i = 0;i < coord->array->int_array.size();i++) coord->array->int_array.push_back(coord->array->int_array[i]);
	for(unsigned int i = 0;i < coord->array->double_array.size();i++) coord->array->double_array.push_back(coord->array->double_array[i]);
	for(unsigned int i = 0;i < coord->array->double_array.size();i++) coord->array->double_array.push_back(coord->array->double_array[i]);
        for(unsigned int i = 0;i < model->array->int_array.size();i++) model->array->int_array.push_back(model->array->int_array[i]);
        for(unsigned int i = 0;i < model->array->double_array.size();i++) model->array->double_array.push_back(model->array->double_array[i]);
        for(unsigned int i = 0;i < model->array->double_array.size();i++) model->array->double_array.push_back(model->array->double_array[i]);
}

void client_t::close(){
	//coord->close();
	//model->close();
	delete coord;
	delete model;
	coord = nullptr;
	model = nullptr;
}
void net_data_t::init(){
	ip = "";
	port = 0;
}
