#include "server_physics.h"

extern std::vector<coord_t*> coord;
extern std::vector<coord_extra_t> coord_extra;
extern std::vector<model_t*> model;
extern std::vector<model_extra_t> model_extra;

extern thread_t *thread;

static unsigned long int coord_size;

static bool same_sign(const double*,const double*);
static void coord_physics_iteration(coord_t*);

#define X_PLANE 'x'
#define Y_PLANE 'y'
#define Z_PLANE 'z'
#define THREAD_COUNT 24

static bool overlap_single_plane(const double *a){ // 0 & 1 are the minimum and maximum for one, 2 & 3 are the same for the other respectively
	return (a[0] >= a[4] && a[1] <= a[3]);
}

static bool overlap_all_planes(double *a, double *b, double *c){
	return overlap_single_plane(a) && overlap_single_plane(b) && overlap_single_plane(c);
}

static void coord_get_model_size(coord_t *a, double *x, double *y, double *z){
	x[0] = x[1] = a->x;
	y[0] = y[1] = a->y;
	z[0] = z[1] = a->z;
	if(a->model != nullptr){
		a->model->get_size(&x[0],&y[0],&z[0]);
	}
}

static bool overlap(coord_t *a, coord_t *b){
	double x[4],y[4],z[4];
	coord_get_model_size(a,x,y,z);
	coord_get_model_size(b,&x[2],&y[2],&z[2]);
	return overlap_all_planes(x,y,z);
}

static bool same_sign(const double *a, const double *b){
	if((*a > 0 && *b < 0) || (*a < 0 && *b > 0)){
		return false;
	}
	return true;
}

static void interaction_overlap_correct_velocities(double *a, double *b){
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

static void sanity_check_inner_loop(coord_t *a, coord_t *b){
}

static void physics_engine_loop(unsigned long int *i){
	for(unsigned long int n = *i+1;n < coord_size;n++){
		interaction(coord[*i],coord[n]);
		sanity_check_inner_loop(coord[*i],coord[n]);
	}
	coord_physics_iteration(coord[*i]);
}

static void physics_engine_loop_section(unsigned long int start, unsigned int long end){
	for(unsigned long int i = start;i < end;i++){
		physics_engine_loop(&i);
	}
}

void physics_engine(){
	coord_size = coord.size();
	if(coord_size > 1024){
		printf("Coord size is above 1024, switching to all multithread mode to speed things along a bit\n");
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
	coord[coord.size()-1]->init();
	coord_extra_t b;
	coord_extra.push_back(b);
	term_if_true(coord_extra.size() != coord.size(),const_cast<char*>("coord_extra.size() != coord.size()"));
}

void delete_close_coord_t(coord_t *a){
	for(unsigned long int i = 0;i < coord_size;i++){
		if(a == coord[i]){
			delete coord[i];
			a = coord[i] = nullptr;
			coord.erase(coord.begin()+i);
			break;
		}
	}
}

void new_init_model_t(){
	model.push_back(new model_t);
	model[model.size()-1]->init();
	model_extra_t b;
	model_extra.push_back(b);
	term_if_true(model_extra.size() != model.size(),(char*)"model sizes do not match\n");
}

void delete_close_model_t(model_t *a){
	const int model_size = model.size();
	for(int i = 0;i < model_size;i++){
		if(a == model[i]){
			delete a;
			a = model[i] = nullptr;
			model.erase(model.begin()+i);
			break;
		}
	}
}

static void coord_physics_iteration(coord_t *a){
	term_if_true(a == nullptr || a == nullptr,(char*)"coord_physics_iteration a is NULL\n");
	double time = get_time();
	a->physics_time = time-a->old_time;
	a->old_time = time;
	if(a->physics_time > 0){
		if(a->mobile){
			a->x += a->x_vel*a->physics_time;
			a->y_vel += GRAVITY_RATE*a->physics_time;
			a->y += a->y_vel*a->physics_time;
			a->z += a->z_vel*a->physics_time;
		}else{
			a->x_vel = a->y_vel = a->z_vel = 0;
		}
	}
}
