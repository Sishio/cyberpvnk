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
#include "server_main.h"
#include "server_physics.h"

extern int scan_model_for_id(int);
extern bool terminate;
static void coord_physics_iteration(coord_t*);

extern bool once_per_second;
extern loop_t server_loop_code;

static void apply_input(coord_t *a, int event){
	a->array.data_lock.lock();
	const long double sin_y_rot = sin(a->y_angle);
	//const long double sin_x_rot = sin(a->x_angle);
	const long double cos_y_rot = cos(a->y_angle);
	const long double cos_x_rot = cos(a->x_angle);
	switch(event){
	case INPUT_MOTION_FORWARD:
		a->x_vel += sin_y_rot*cos_x_rot;
		a->z_vel -= cos_y_rot*cos_x_rot;
		break;
	case INPUT_MOTION_BACKWARD:
		a->x_vel -= sin_y_rot*cos_x_rot;
		a->y_vel += cos_y_rot*cos_x_rot;
		break;
	case INPUT_MOTION_LEFT:
		a->x_vel -= cos_y_rot;
		a->z_vel -= sin_y_rot;
		break;
	case INPUT_MOTION_RIGHT:
		a->x_vel += cos_y_rot;
		a->z_vel += sin_y_rot;
		break;
	default:
		break;
	}
	a->array.data_lock.unlock();
}

static input_settings_t *default_input_settings_pointer = nullptr;

static void coord_physics_apply_input(coord_t *a, input_buffer_t *b, input_settings_t *input_settings = nullptr){
	if(b == nullptr || a == nullptr){
		return;
	}
	b->array.data_lock.lock();
	a->array.data_lock.lock();
	int search_integer = 0;
	switch(b->type){
	case INPUT_TYPE_KEYBOARD:
		search_integer = b->int_data[INPUT_TYPE_KEYBOARD_CHAR];
		break;
	default:
		break;
	}
	bool no_input_settings = false;
	if(input_settings == nullptr){
		no_input_settings = true;
		if(default_input_settings_pointer == nullptr){
			default_input_settings_pointer = new input_settings_t;
			printf("Warning: no input_settings provided, using the default values\n");
			default_input_settings_pointer->int_data[0][0] = 'w';
			default_input_settings_pointer->int_data[0][1] = INPUT_MOTION_FORWARD;
			default_input_settings_pointer->int_data[1][0] = 'a';
			default_input_settings_pointer->int_data[1][1] = INPUT_MOTION_LEFT;
			default_input_settings_pointer->int_data[2][0] = 's';
			default_input_settings_pointer->int_data[2][1] = INPUT_MOTION_BACKWARD;
			default_input_settings_pointer->int_data[3][0] = 'd';
			default_input_settings_pointer->int_data[3][1] = INPUT_MOTION_RIGHT;
		}
		input_settings = default_input_settings_pointer;
	}
	for(unsigned long int i = 0;i < 64;i++){
		if(input_settings->int_data[i][0] == search_integer){
			a->array.data_lock.unlock();
			apply_input(a, input_settings->int_data[i][1]);
			a->array.data_lock.lock();
			break;
		}
	}
	b->array.data_lock.unlock();
	a->array.data_lock.unlock();
}

static void apply_all_input(){
	std::vector<void*> tmp_input_buffer = all_pointers_of_type("input_buffer_t");
	const unsigned long int tmp_input_buffer_size = tmp_input_buffer.size();
	for(unsigned long int i = 0;i < tmp_input_buffer_size;i++){
		client_t *tmp_client = (client_t*)find_pointer(((input_buffer_t*)tmp_input_buffer[i])->client_id, "client_t");
		if(tmp_client != nullptr){
			coord_physics_apply_input((coord_t*)find_pointer(tmp_client->coord_id, "coord_t"), (input_buffer_t*)tmp_input_buffer[i]);
			delete (input_buffer_t*)tmp_input_buffer[i];
		}
		tmp_client = nullptr;
	}
}

void physics_init(std::string gametype_load = "gametype/default"){
	loop_add(&server_loop_code, "physics_engine", physics_engine);
	new gametype_t(gametype_load);
}

void physics_close(){
	loop_del(&server_loop_code, physics_engine);
}

void physics_engine(){
	once_per_second_update();
	apply_all_input();
	std::vector<void*> coord_buffer = all_pointers_of_type("coord_t");
	const unsigned long int coord_size = coord_buffer.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		coord_physics_iteration((coord_t*)coord_buffer[i]);
	}
}

static void coord_physics_iteration(coord_t *a){
	term_if_true(a == nullptr,(char*)"coord_physics_iteration a is NULL/nullptr\n");
	a->array.data_lock.lock();
	const long double time = get_time();
	a->physics_time = time-(a->old_time);
	a->old_time = time;
	if(a->mobile){
		a->x += a->x_vel*a->physics_time;
		a->y_vel += GRAVITY_RATE*a->physics_time;
		a->y += a->y_vel*a->physics_time;
		a->z += a->z_vel*a->physics_time;
		const long double pos_mul = 1/(1+(.9*a->physics_time));
		a->x_vel *= pos_mul;
		a->y_vel *= pos_mul;
		a->z_vel *= pos_mul;
	}
	a->x_angle += a->x_angle_vel*a->physics_time;
	a->y_angle += a->y_angle_vel*a->physics_time;
	const long double angle_mul = 1/(1+(.1*a->physics_time));
	a->x_angle_vel *= angle_mul;
	a->y_angle_vel *= angle_mul;
	a->array.data_lock.unlock();
}
