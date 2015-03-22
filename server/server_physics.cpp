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

/*static void apply_input(coord_t *a, int event){
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
}*/

void physics_init(){
	loop_add(&server_loop_code, "physics engine", physics_engine);
}

void physics_close(){
	loop_del(&server_loop_code, physics_engine);
}

void physics_engine(){
	once_per_second_update();
	std::vector<void*> coord_buffer = all_pointers_of_type("coord_t", true);
	for(unsigned long int i = 0;i < coord_buffer.size();i++){
		coord_physics_iteration((coord_t*)coord_buffer[i]);
	}
}

static void coord_physics_iteration(coord_t *a){
	//a->array.data_lock.lock();
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
