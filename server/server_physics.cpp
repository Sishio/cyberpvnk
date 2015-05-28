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

static int_ physics_rules_type;

// 3D physics

static physics_rules_t *rules = nullptr;
static array_id_t rules_id = 0;

namespace coord_3d_input_functions{
	void forward(coord_t*);
	void backward(coord_t*);
	void left(coord_t*);
	void right(coord_t*);
	void up(coord_t*);
	void down(coord_t*);
}

void coord_3d_input_functions::forward(coord_t *a){
	const long double y_sin = sin(a->y_angle);
	const long double y_cos = cos(a->y_angle);
	const long double x_cos = cos(a->x_angle);
	a->x_vel += y_sin*x_cos*a->physics_time;
	a->z_vel -= y_cos*x_cos*a->physics_time;
}

void coord_3d_input_functions::backward(coord_t *a){
	const long double y_sin = sin(a->y_angle);
	const long double y_cos = cos(a->y_angle);
	const long double x_cos = cos(a->x_angle);
	a->x_vel -= y_sin*x_cos*a->physics_time;
	a->z_vel += y_cos*x_cos*a->physics_time;
}

void coord_3d_input_functions::left(coord_t *a){
	const long double y_sin = sin(a->y_angle);
	const long double y_cos = cos(a->y_angle);
	a->x_vel -= y_cos*a->physics_time;
	a->z_vel -= y_sin*a->physics_time;
}

void coord_3d_input_functions::right(coord_t *a){
	const long double y_sin = sin(a->y_angle);
	const long double y_cos = cos(a->y_angle);
	a->x_vel += y_cos*a->physics_time;
	a->z_vel += y_sin*a->physics_time;
}

void coord_3d_input_functions::up(coord_t *a){
	const long double x_sin = sin(a->x_angle);
	a->x_vel += x_sin*a->physics_time;
}

void coord_3d_input_functions::down(coord_t *a){
	const long double x_sin = sin(a->x_angle);
	a->x_vel -= x_sin*a->physics_time;
}

namespace coord_2d_input_functions{
	void up(coord_t*);
	void down(coord_t*);
	void left(coord_t*);
	void right(coord_t*);
}

void coord_2d_input_functions::up(coord_t* a){
	a->y_vel += a->physics_time*rules->get_acceleration();
}

void coord_2d_input_functions::down(coord_t* a){
	a->y_vel -= a->physics_time*rules->get_acceleration();
}

void coord_2d_input_functions::left(coord_t *a){
	a->x_vel -= a->physics_time*rules->get_acceleration();
}

void coord_2d_input_functions::right(coord_t *a){
	a->x_vel += a->physics_time*rules->get_acceleration();
}

void physics_init(){
	loop_add(&server_loop_code, "physics engine", physics_engine);
}

void physics_close(){
	loop_del(&server_loop_code, physics_engine);
}

std::vector<array_id_t> coord_vector;

static void coord_collision_check(coord_t*);
static bool tile_overlap(array_id_t, array_id_t){return false;}
static bool model_overlap(array_id_t, array_id_t){return false;}

void coord_loop(array_id_t a){
	coord_t *a_ = (coord_t*)find_pointer(a, "coord_t");
	if(a_ == nullptr){
		return;
	}
	a_->array.data_lock.lock();
	coord_physics_iteration(a_);
	coord_collision_check(a_);
	a_->array.data_lock.unlock();
}

void physics_engine(){
	once_per_second_update();
	std::vector<array_id_t> coord_vector = all_ids_of_type("coord_t");
	// pull the client's input and work that into here
	for(uint_ i = 0;i < coord_vector.size();i++){
		coord_loop(coord_vector[i]);
	}
}

static void coord_collision_check(coord_t *coord){
	assert(coord->array.data_lock.try_lock() == false);
	if(coord->get_interactable() == false){
		printf_("WARNING: Attempted a coord_collision_check with an non-interactable object\n", PRINTF_LIKELY_WARN);
	}
	const uint_ coord_vector_size = coord_vector.size();
	for(uint_ i = 0;i < coord_vector_size;i++){
		coord_t *current_coord = (coord_t*)find_pointer(coord_vector[i]);
		bool collision = false;
		if(unlikely(coord->array.id == current_coord->array.id) || current_coord->get_interactable() == false){
			collision = false;
		}else{
			bool use_model = current_coord->model_id != 0;
			bool use_tile = current_coord->tile_id != 0;
			if(!use_model && !use_tile){
				printf_("WARNING: Attempting to check for a collision on a model-less and tile-less object. This hasn't been implemented yet.\n", PRINTF_LIKELY_WARN);
			}else if(use_tile){
				if(coord->tile_id != 0){
					collision = tile_overlap(coord->tile_id, current_coord->tile_id);
				}else{
					printf_("WARNING: 3D and 2D models are interacting with each other\n", PRINTF_UNLIKELY_WARN);
					collision = false;
				}
			}else if(use_model){
				if(coord->model_id != 0){
					collision = model_overlap(coord->model_id, current_coord->model_id);
				}else{
					printf_("WARNING: 3D and 2D models are interacting with each other\n", PRINTF_UNLIKELY_WARN);
					collision = false;
				}
			}
		}
		if(collision){
			coord->x_vel = -coord->x_vel;
			coord->y_vel = -coord->y_vel;
			coord->z_vel = -coord->z_vel;
			current_coord->x_vel = -current_coord->x_vel;
			current_coord->y_vel = -current_coord->y_vel;
			current_coord->z_vel = -current_coord->z_vel;
		}else{
			printf_("WARNING: Couldn't make the two object collide\n", PRINTF_UNLIKELY_WARN);
		}
	}
}

static void coord_physics_iteration(coord_t *a){ // don't send this until I find an easy write protection thing
	const long double time = get_time();
	a->physics_time = time-(a->old_time);
	a->old_time = time;
	if(a->mobile){
		a->x += a->x_vel*a->physics_time;
		a->y_vel += rules->get_gravity()*a->physics_time;
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
}

physics_rules_t::physics_rules_t() : array(this, false){
	array.data_type = "physics_rules_t";
	array.long_double_array.push_back(&acceleration);
	array.long_double_array.push_back(&mysterious_downward_force);
	acceleration = 1;
	mysterious_downward_force = 9.81/2; // two meters per 16*16 block
}

long double physics_rules_t::get_acceleration(){return acceleration;}

long double physics_rules_t::get_gravity(){return mysterious_downward_force;}

physics_rules_t::~physics_rules_t(){
}
