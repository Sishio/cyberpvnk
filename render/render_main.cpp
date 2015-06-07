#include "../class/class_array.h"
#include "../class/class_main.h"
#include "render_main.h"

tile_t *no_tile_tile = nullptr;

array_id_t search_for_image(std::string image_path){
	std::vector<array_id_t> all_images = all_ids_of_type("image_t");
	const uint_ all_images_size = all_images.size();
	for(uint_ i = 0;i < all_images_size;i++){
		try{
			image_t *tmp = (image_t*)find_pointer(all_images[i]);
			throw_if_nullptr(tmp);
			if(tmp->get_filename() == image_path){ // find_first_of would work pretty well here
				return all_images[i];
			}
		}catch(const std::logic_error &e){}
	}
	printf_("STATUS: Generating a new image since the requested image hasn't been created yet\n", PRINTF_STATUS);
	return (new image_t(image_path))->array.id;
}

/*static void load_all_images(){
	std::string image_directory = "../image_data/";
	#ifdef __linux
	system(("find " + image_directory + " > image_list").c_str());
	#elif _WIN32
	#endif
	std::ifstream in("image_list");
	if(in.is_open() == false){
		printf_("ERROR: Couldn't open the image directory\n", PRINTF_VITAL);
		return;
	}
	char image_list_line[512];
	while(in.getline(image_list_line, 511)){
		image_list_line[511] = '\0';
		std::string line = image_list_line;
		new image_t(image_directory + line); // filename == path
	}
	in.close();
}*/

render_t::render_t(int_ argc, char** argv) : array(this, "render_t", ARRAY_SETTING_IMMUNITY){
	array.set_setting(ARRAY_SETTING_SEND, false);
	array.set_setting(ARRAY_SETTING_IMMUNITY, true);
	// settings are technically a part of the array_t type and aren't blanked
	array.reset_values();
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf_("ERROR: SDL2 Video couldn't be initialized", PRINTF_VITAL);
		return;
	}
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	if(no_tile_tile == nullptr){
		no_tile_tile = new tile_t;
		no_tile_tile->array.set_setting(ARRAY_SETTING_IMMUNITY, true);
		array_id_t no_tile_image = all_ids_of_type("image_t")[0];
		printf_("STATUS: no_tile_image (ID) == " + std::to_string(no_tile_image) + "\n", PRINTF_STATUS);
		for(uint_ i = 0;i < TILE_IMAGE_SIZE; i++){
			no_tile_tile->set_image_id(0, i, no_tile_image);
		}
		no_tile_tile->set_current_animation_entry(0);
	}
}

void render_t::loop(){
	if(screen.size() == 0){
		printf_("WARNING: There is not a loaded screen yet. FIX THIS\n", PRINTF_UNLIKELY_WARN);
		return;
	}
	screen_t *tmp_screen = nullptr;
	try{
		tmp_screen = (screen_t*)find_pointer(screen[0]);
		throw_if_nullptr(tmp_screen);
	}catch(std::logic_error &e){
		printf_("WARNING: the screen is a nullptr\n", PRINTF_UNLIKELY_WARN);
		return;
	}
	tmp_screen->array.data_lock.lock();
	std::vector<array_id_t> coord_vector = all_ids_of_type("coord_t");
	for(uint_ i = 0;i < coord_vector.size();i++){
		try{
			coord_t *tmp = (coord_t*)find_pointer(coord_vector[i]);
			throw_if_nullptr(tmp);
			tile_t *tmp_tile = (tile_t*)find_pointer(tmp->tile_id);
			throw_if_nullptr(tmp_tile);
			tmp_screen->write_to_screen(tmp);
		}catch(std::logic_error &e){
			printf_("SPAM: render_t::loop(): coord_vector ID entry '" "' turned into a nullptr since it was received with all_ids_of_type\n", PRINTF_SPAM);
		}
	}
	if(tmp_screen->renderer != nullptr){
		SDL_RenderPresent(tmp_screen->renderer);
		SDL_RenderClear(tmp_screen->renderer);
	}else{
		printf_("ERROR: tmp_screen->renderer == nullptr\n", PRINTF_ERROR);
	}
	tmp_screen->array.data_lock.unlock();
}

render_t::~render_t(){
	IMG_Quit();
}

screen_t::screen_t() : array(this , "screen_t", ARRAY_SETTING_IMMUNITY){
	array.int_array.push_back(std::make_pair(&x_res, "x res."));
	array.int_array.push_back(std::make_pair(&y_res, "y res."));
	array.string_array.push_back(std::make_pair(&title, "title"));
	array.void_ptr_array.push_back(std::make_pair((void**)&screen, "pointer to screen"));
	array.void_ptr_array.push_back(std::make_pair((void**)&screen_surface, "pointer to screen_surface"));
	array.void_ptr_array.push_back(std::make_pair((void**)&renderer, "pointer to renderer"));
	array.reset_values();
	x_res = 640;
	y_res = 480;
	title = "No Title";
	std::cout << "screen_t ID: " << array.id << std::endl;
}

void screen_t::new_screen(){
	try{
		screen = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x_res, y_res, SDL_WINDOW_SHOWN);
		throw_if_nullptr(screen);
		screen_surface = SDL_GetWindowSurface(screen);
		throw_if_nullptr(screen_surface);
		renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
		throw_if_nullptr(renderer);
	}catch(std::logic_error &e){
		printf_("ERROR: Couldn't initialize the screen\n", PRINTF_ERROR);
	}
}

void screen_t::del_screen(){
	if(screen_surface != nullptr){
		SDL_FreeSurface(screen_surface);
		screen_surface = nullptr;
	}
	if(screen != nullptr){
		SDL_DestroyWindow(screen);
		screen = nullptr;
	}
}

tile_t::tile_t() : array(this, "tile_t", ARRAY_SETTING_SEND){
	for(int c = 0;c < TILE_ANIMATION_SIZE;c++){
		for(int i = 0;i < TILE_IMAGE_SIZE;i++){
			image[c][i] = 0;
			array.int_array.push_back(std::make_pair(&image[c][i], "tile image " + std::to_string(c) + " " + std::to_string(i) + " "));
		}
	}
	//memset(image, 0, sizeof(array_id_t)*TILE_ANIMATION_SIZE*TILE_IMAGE_SIZE);
	current_animation_entry = 0;
	array.int_array.push_back(std::make_pair(&current_animation_entry, "current animation entry"));
}

array_id_t tile_t::get_image_id(uint_ c, uint_ a){
	if(c > TILE_ANIMATION_SIZE || a > TILE_IMAGE_SIZE){
		printf_("ERROR: get_image_id: requested out of bounds data\n", PRINTF_ERROR);
		assert(false);
		return 0;
	}
	return image[c][a];
}

uint_ tile_t::get_current_animation_entry(){
	return current_animation_entry;
}

void tile_t::set_current_animation_entry(uint_ a){
	if(a > TILE_ANIMATION_SIZE){
		printf_("ERROR: current_animation_entry > TILE_ANIMATION_SIZE.\n", PRINTF_ERROR);
		a = 0;
	}
	current_animation_entry = a;
}

array_id_t tile_t::get_current_image_id(){
	double int_part = 0, get_time_ = get_time();
	uint_ entry = (modf(get_time_, &int_part)*TILE_IMAGE_SIZE)-.5;
	return get_image_id(get_current_animation_entry(), entry);
}

void tile_t::set_image_id(uint_ tmp, uint_ entry, array_id_t id){
	if(entry > TILE_IMAGE_SIZE){
		printf_("ERROR: entry > TILE_IMAGE_SIZE\n", PRINTF_ERROR);
		entry = 0;
	}
	if(tmp > TILE_ANIMATION_SIZE){
		printf_("ERROR: tmp > TILE_ANIMATION_SIZE\n", PRINTF_ERROR);
		tmp = 0;
	}
	image[tmp][entry] = id;
}

tile_t::~tile_t(){
}

image_t::image_t(std::string filename_) : array(this, "image_t", ARRAY_SETTING_SEND){
	filename = filename_;
	array.string_array.push_back(std::make_pair(&filename, "filename"));
	array.data_type = "image_t";
	surface = IMG_Load(filename_.c_str());
	texture = nullptr;
	if(surface == NULL){
		printf_("ERROR: image_t::image_t: Couldn't load '" + filename_ + "' as an SDL_Surface.\n", PRINTF_ERROR);
	}else{
		printf_("STATUS: image_t::image_t: Loaded '" + filename_ + "'\n", PRINTF_STATUS);
		render_t *tmp_render = (render_t*)find_pointer(all_ids_of_type("render_t")[0]);
		throw_if_nullptr(tmp_render);
		screen_t *tmp_screen = tmp_render->get_current_screen();
		throw_if_nullptr(tmp_screen);
		texture = SDL_CreateTextureFromSurface(tmp_screen->renderer, surface);
		throw_if_nullptr(texture);
	}
}

image_t::~image_t(){
	if(surface != nullptr){
		SDL_FreeSurface(surface);
	}
}

void screen_t::write_to_screen(coord_t* tmp){
	if(screen_surface == nullptr){
		printf_("ERROR: screen_surface == nullptr\n", PRINTF_ERROR);
		return;
	}
	SDL_Rect dest;
	dest.x = tmp->x-8;
	dest.y = tmp->y-8; // 1/2 of the width and height makes the center coord
	dest.w = 16; // tile size is 16*16
	dest.h = 16;
	try{
		tile_t *tmp_tile = (tile_t*)find_pointer(tmp->tile_id);
		throw_if_nullptr(tmp_tile);
		image_t *tmp_image = (image_t*)find_pointer(tmp_tile->get_current_image_id());
		throw_if_nullptr(tmp_image);
		SDL_RenderCopy(renderer, tmp_image->get_texture(), NULL, &dest);
	}catch(std::logic_error &e){}
}

std::string image_t::get_filename(){
	return filename;
}

SDL_Surface* image_t::get_surface(){
	return surface;
}

SDL_Texture* image_t::get_texture(){
	if(surface != nullptr && texture == nullptr){
		render_t *tmp_render = nullptr;
		screen_t *tmp_screen = nullptr;
		SDL_Renderer *tmp_renderer = nullptr;
		try{
			tmp_render = (render_t*)find_pointer(all_ids_of_type("render_t")[0]);
			throw_if_nullptr(tmp_render);
			tmp_screen = tmp_render->get_current_screen();
			throw_if_nullptr(tmp_screen);
			tmp_renderer = tmp_screen->renderer;
			throw_if_nullptr(tmp_renderer);
			texture = SDL_CreateTextureFromSurface(tmp_renderer, surface);
		}catch(std::logic_error &e){
			texture = nullptr;
		}
	}
	if(texture == nullptr){
		printf_("DEBUG: get_texture: couldn't generate a texture (texture == nullptr)\n", PRINTF_DEBUG);
	}
	return texture;
}

screen_t* render_t::get_current_screen(){
	return (screen.size() == 0)?(nullptr):((screen_t*)find_pointer(screen[0]));
}
