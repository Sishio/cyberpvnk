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

extern std::vector<coord_t*> coord;
extern std::vector<coord_extra_t> coord_extra;
extern std::vector<model_t*> model;
extern std::vector<model_extra_t> model_extra;
extern int scan_model_for_id(int);
extern bool terminate;

extern thread_t *thread;

static unsigned long int coord_size;
static unsigned long int model_size;

static void coord_physics_iteration(coord_t*);

static long double once_per_second_time = 0;
static bool once_per_second = false;

#define X_PLANE 'x'
#define Y_PLANE 'y'
#define Z_PLANE 'z'

static void once_per_second_update(){
	if(once_per_second_time > 1){
		while(once_per_second_time > 1){
			printf("once_per_second_update(): my god this is slow\n");
			once_per_second_time -= 1;
		}
		once_per_second = true;
		printf("OPS code will run. OPS CLOCK: %Lf\n",once_per_second_time);
	}else{
		once_per_second = false;
	}
}

static void update_vector_sizes(){
	coord_size = coord_vector.size();
	model_size = model_vector.size();
}

static void coord_physics_apply_input(coord_t *a, input_buffer_t *b){
	input_settings_t *input_settings = new input_settings_t;
	switch(b->type){
	case INPUT_TYPE_KEYBOARD:
		for(unsigned long int i = 0;i < 64;i++){
			bool detected = input_settings->int_data[i][0] == b->int_data[INPUT_TYPE_KEYBOARD_KEY] || (b->int_data[INPUT_TYPE_KEYBOARD_CHAR] != 0 && input_settings->int_data[i][0] == b->int_data[INPUT_TYPE_KEYBOARD_CHAR]);
			if(detected){
				a->apply_motion(input_settings->int_data[i][1]);
			}
		}
		break;
	case INPUT_TYPE_MOUSE_MOTION:
		break;
	case INPUT_TYPE_MOUSE_PRESS:
		break;
	}
	delete input_settings;
	input_settings = nullptr;
}

static void apply_all_input(){
	for(unsigned long int i = 0;i < input_buffer_vector.size();i++){
		printf("Applying input\n");
		client_t *tmp_client = find_client_pointer(input_buffer_vector[i]->client_id);
		term_if_true(tmp_client == nullptr, (char*)"tmp_client is nullptr");
		coord_physics_apply_input(find_coord_pointer(tmp_client->coord_id), input_buffer_vector[i]);
		input_buffer_vector.erase(input_buffer_vector.begin()+i);
	}
}

void physics_init(){
	loop_code.push_back(physics_engine);
}

void physics_close(){}

void physics_engine(){
	update_vector_sizes();
	once_per_second_update();
	apply_all_input();
	for(unsigned long int i = 0;i < coord_size;i++){
		coord_physics_iteration(coord_vector[i]);
	}
}

static void coord_physics_iteration(coord_t *a){
	term_if_true(a == nullptr,(char*)"coord_physics_iteration a is NULL/nullptr\n"); // the overhead is minimal
	long double time = get_time();
	a->physics_time = time-(a->old_time);
	a->old_time = time;
	if(a->mobile){
		a->x += a->x_vel*a->physics_time;
		a->y_vel += GRAVITY_RATE*a->physics_time;
		a->y += a->y_vel*a->physics_time;
		a->z += a->z_vel*a->physics_time;
	}else{
		a->x_vel = a->y_vel = a->z_vel = 0;
	}
}
