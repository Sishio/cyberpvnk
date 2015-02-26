#include "server_render.h"
#include "server_main.h"

static bool render_init_bool = false;
extern server_info_t *server_info;
extern coord_t *map;
extern loop_t server_loop_code;

void render_init(){
	render_init_bool = true;
	if(map == nullptr){
		printf("WARNING: map == nullptr, make sure that the physics_init function ran BEFORE the render_init function\n");
		map = new coord_t;
	}
	model_t *tmp_model = new model_t;
	map->model_id = tmp_model->array.id;
	assert(server_info != nullptr);
	model_load(tmp_model, server_info->map_name);
	loop_add(&server_loop_code, render_engine);
}

void render_engine(){
	if(unlikely(render_init_bool == false)){
		render_init();
	}
	const unsigned long int array_size = array_vector.size();
	for(unsigned long int i = 0;i < array_size;i++){
		if(array_vector[i]->data_type == "render_buffer_t"){
			const long int coord_id = ((render_buffer_t*)array_vector[i]->pointer)->coord_id;
			long int model_id;
			if(((client_t*)array_vector[i]->pointer)->model_id == 0){
				model_id = ((coord_t*)find_pointer(coord_id))->model_id;
			}else{
				model_id = ((coord_t*)find_pointer(coord_id))->model_id;
			}
			bool found_client = false;
			for(unsigned long int n = 0;n < array_size;n++){
				if(array_vector[n]->data_type == "render_buffer_t"){
					render_buffer_t *tmp_render_buffer = ((render_buffer_t*)array_vector[n]->pointer);
					const bool condition_a = tmp_render_buffer->coord_id == coord_id;
					const bool condition_b = tmp_render_buffer->model_id == model_id;
					if(unlikely(condition_a && condition_b)){
						found_client = true;
						break;
					}
				}
			}
			if(found_client == false){
				render_buffer_t *tmp_render_buffer = new render_buffer_t;
				tmp_render_buffer->coord_id = coord_id;
				tmp_render_buffer->model_id = model_id;
			}
		}
	}
}

void render_close(){
	const unsigned long int array_size = array_vector.size();
	for(unsigned long int i = 0;i < array_size;i++){
		if(array_vector[i]->data_type == "render_buffer_t"){
			delete (render_buffer_t*)array_vector[i]->pointer;
		}
	}
}
