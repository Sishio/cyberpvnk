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

static bool same_sign(const long double*,const long double*);
static void coord_physics_iteration(coord_t*);

static long double total_time = 0;
static long double once_per_second_time = 0;
static bool once_per_second = false;

#define X_PLANE 'x'
#define Y_PLANE 'y'
#define Z_PLANE 'z'

static bool overlap_single_plane(const long double *a){ // 0 & 1 are the minimum and maximum for one, 2 & 3 are the same for the other respectively
	return (a[0] >= a[4] && a[1] <= a[3]);
}

static bool overlap_all_planes(long double *a, long double *b, long double *c){
	return overlap_single_plane(a) && overlap_single_plane(b) && overlap_single_plane(c); // re-order this to be more efficient
}

static void coord_get_model_size(coord_t *a, long double *x, long double *y, long double *z){
	x[0] = x[1] = a->x;
	y[0] = y[1] = a->y;
	z[0] = z[1] = a->z;
	if(a->model_id != -1){
		int model_entry = scan_model_for_id(a->model_id);
		if(model_entry != -1){
			model[model_entry]->get_size(&x[0],&y[0],&z[0]);
		}
	}
}

static void overlap_include_give_or_take(long double *a, long double b){
	a[0] -= b;
	a[1] += b;
}

static bool overlap(coord_t *a, coord_t *b, long double give_or_take = 0){
	long double x[4],y[4],z[4];
	coord_get_model_size(a,x,y,z);
	coord_get_model_size(b,&x[2],&y[2],&z[2]);
	if(give_or_take != 0){
		overlap_include_give_or_take(x, give_or_take);
		overlap_include_give_or_take(&x[2],give_or_take);
		overlap_include_give_or_take(y, give_or_take);
		overlap_include_give_or_take(&y[2],give_or_take);
		overlap_include_give_or_take(z, give_or_take);
		overlap_include_give_or_take(&z[2],give_or_take);
	}
	return overlap_all_planes(x,y,z);
}

static bool same_sign(const long double *a, const long double *b){
	if((*a > 0 && *b < 0) || (*a < 0 && *b > 0)){
		return false;
	}
	return true;
}

static void interaction_overlap_correct_velocities(long double *a, long double *b){
	if(same_sign(a,b)){
		if(*a > *b){
			*b = *a;
		}else{
			*a = *b;
		}
	}else{
		*a *= -1;
		*b *= -1;
	}
}

static void interaction(coord_t *a, coord_t *b){
	if(overlap(a,b)){
		interaction_overlap_correct_velocities(&a->x_vel,&b->x_vel);
		interaction_overlap_correct_velocities(&a->y_vel,&b->y_vel);
		interaction_overlap_correct_velocities(&a->z_vel,&b->z_vel);
	}
}

static bool coord_check_if_nearby_in_vector(std::vector<unsigned long int> *a, unsigned long int b, unsigned int *c){
	unsigned int a_size = a->size();
	for(*c = 0;*c < a_size;*c += 1){
		if((*a)[*c] == b){
			return true;
		}
	}
	return false;
}

static void coord_check_if_nearby_update(std::vector<unsigned long int> *a, unsigned long int b, bool c){
	unsigned int e;
	bool d = coord_check_if_nearby_in_vector(a,b,&e);
	if(!d && c){ // if not there and supposed to be
		a->push_back(b); // put it there
	}
	if(d && !c){ // if it is there and it is not supposed to be (anymore)
		a->erase(a->begin()+e);
	}
}

static void coord_check_if_nearby(unsigned long int i, unsigned long int n){
	bool overlap_ = overlap(coord[i],coord[n],5); // should it be there?
	coord_check_if_nearby_update(&coord[i]->nearby_coord,n,overlap_);
	coord_check_if_nearby_update(&coord[n]->nearby_coord,i,overlap_);
}

static void physics_engine_loop_ops(){
	for(unsigned long int i = 0;i < coord_size;i++){
		for(unsigned long int n = i+1;n < coord_size;n++){
			coord_check_if_nearby(i,n);
			if(unlikely(terminate)){
				return;
			}
		}
	}
}

static void physics_engine_loop(unsigned long int *i){
	const unsigned long int i_size = coord[*i]->nearby_coord.size();
	for(unsigned long int n = 0;n < i_size;n++){
		interaction(coord[*i],coord[n]);
	}
	coord_physics_iteration(coord[*i]);
}

static void physics_engine_loop_section(unsigned long int start, unsigned int long end){
	for(unsigned long int i = start;i < end;i++){
		physics_engine_loop(&i);
	}
}

static void once_per_second_update(){
	if(once_per_second_time > 1){
		if(once_per_second_time > 100) once_per_second_time = 1.1; // first iteration tends to not make sense
		once_per_second_time -= 1;
		once_per_second = true;
		printf("OPS code will run. OPS CLOCK: %Lf\n",once_per_second_time);
	}else{
		once_per_second = false;
	}
}

static void update_vector_sizes(){
	coord_size = coord.size();
	model_size = model.size();
}

void physics_engine(){
	update_vector_sizes();
	if(likely(coord_size > 0)){
		printf("physics_time:%Lf\n",coord[0]->physics_time);
		total_time += coord[0]->physics_time;
		once_per_second_time += coord[0]->physics_time;
		once_per_second_update();
	}
	if(once_per_second){
		if(coord_size > OPS_THRESHOLD){ // I need to make a header file full of macros so people can put their own data into here
			static std::thread a(physics_engine_loop_ops);
		}else{
			physics_engine_loop_ops();
		}
	}
	#ifndef DISABLE_PHYSICS_THREAD
	if(coord_size > MAIN_PHYSICS_THRESHOLD){
	#else
	if(false){
	#endif
		unsigned long int section = (unsigned long int)coord_size/THREAD_COUNT; // loops down
		unsigned long int start = 0;
		std::thread *thread[THREAD_COUNT];
		for(int i = 0;i < THREAD_COUNT;i++){
			thread[i] = new std::thread(physics_engine_loop_section,start,section*(i+1));
			start += section;
		}
		if(start < coord_size){
			physics_engine_loop_section(start, coord_size);
		}
		for(int i = 0;i < THREAD_COUNT;i++){
			thread[i]->join();
			delete thread[i];
			thread[i] = nullptr;
		}
	}else{
		physics_engine_loop_section(0,coord_size);
	}
}

void new_init_coord_t(){
	coord.push_back(new coord_t);
	coord_extra_t b;
	coord_extra.push_back(b);
	update_vector_sizes();
	term_if_true(coord_extra.size() != coord.size(),const_cast<char*>("coord_extra.size() != coord.size()"));
}

void delete_close_coord_t(coord_t *a){
	for(unsigned long int i = 0;i < coord_size;i++){
		if(a == coord[i]){
			coord.erase(coord.begin()+i);
			update_vector_sizes();
			delete a;
			a = nullptr;
			break;
		}
	}
}

void new_init_model_t(){
	model.push_back(new model_t);
	model_extra_t b;
	model_extra.push_back(b);
	update_vector_sizes();
	term_if_true(model_extra.size() != model.size(),(char*)"model sizes do not match\n");
}

void delete_close_model_t(model_t *a){
	const int model_size = model.size();
	for(int i = 0;i < model_size;i++){
		if(a == model[i]){
			model.erase(model.begin()+i);
			update_vector_sizes();
			delete a;
			a = nullptr;
			break;
		}
	}
}

static void coord_physics_iteration(coord_t *a){
	term_if_true(a == nullptr,(char*)"coord_physics_iteration a is NULL\n"); // the overhead is minimal
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
