#include "render_main.h"

render_t::render_t(int_ argc, char** argv){
}

render_t::~render_t(){
	
}

tile_t::tile_t() : array(this, true){
	for(int i = 0;i < 128;i++){
		image_id[i] = 0;
		array.int_array.push_back(&image_id[i]);
	}
}

image_t *tile_t::get_image(int a){
	return (image_t*)find_pointer(image_id[a]);
}

image_t *tile_t::get_current_image(){
	return get_image((int)(modf(get_time, nullptr)*10)+.5);
}

array_id_t tile_t::set_image_id(int entry, array_id_t id){
	if(image_id[entry] != 0){
		printf_("WARNING: render_t::set_image_id: the entry that was requested to be filled is occupied\n", PRINTF_WARNING);
	}
	image_id[entry] = id;
}

tile_t::~tile_t(){
}
