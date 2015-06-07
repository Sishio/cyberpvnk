#include "../class/class_array.h"
#include "../class/class_main.h"
#include "render_main.h"

tile_t *no_tile_tile = nullptr;

static void load_all_images(){
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
		new image_t(line);
	}
	in.close();
}

render_t::render_t(int_ argc, char** argv) : array(this, ARRAY_SETTING_SEND){
	array.set_setting(ARRAY_SETTING_SEND, false);
	array.set_setting(ARRAY_SETTING_IMMUNITY, true);
	array.data_type = "render_t";
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf_("ERROR: SDL2 Video couldn't be initialized", PRINTF_VITAL);
		return;
	}
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	load_all_images();
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
		printf_("WARNING: There is not a loaded screen yet. FIX THIS\n", PRINTF_LIKELY_WARN);
		return;
	}
	screen_t *tmp_screen = (screen_t*)find_pointer(screen[0]);
	if(tmp_screen == nullptr){
		printf_("ERROR: The screen ID is a nullptr\n", PRINTF_ERROR);
		return;
	}
	tmp_screen->array.data_lock.lock();
	std::vector<array_id_t> coord_vector = all_ids_of_type("coord_t");
	uint_ i = 0;
	render_loop_loop_start:
	for(;i < coord_vector.size();i++){
		coord_t *tmp = (coord_t*)find_pointer(coord_vector[i]);
		if(tmp == nullptr){
			goto render_loop_loop_start;
		}
		tile_t *tmp_tile = (tile_t*)find_pointer(tmp->tile_id);
		if(tmp_tile == nullptr){
			goto render_loop_loop_start;
		}
		tmp_screen->write_to_screen(tmp);
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

screen_t::screen_t() : array(this, 0){
	array.int_array.push_back(std::make_pair(&x_res, "x res."));
	array.int_array.push_back(std::make_pair(&y_res, "y res."));
	array.string_array.push_back(std::make_pair(&title, "title"));
	screen = nullptr;
	screen_surface = nullptr;
	renderer = nullptr;
	x_res = 640;
	y_res = 480;
	title = "No Title";
}

void screen_t::new_screen(){
	screen = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x_res, y_res, SDL_WINDOW_SHOWN);
	if(screen == nullptr){
		printf_("ERROR: SDL2 Window couldn't be initalized\n", PRINTF_VITAL);
		return;
	}
	screen_surface = SDL_GetWindowSurface(screen);
	if(screen_surface == nullptr){
		printf_("ERROR: SDL2 Window's Surface couldn't be intialized\n", PRINTF_VITAL);
	}
	renderer = SDL_CreateRenderer(screen, -1, 0);
	if(renderer == nullptr){
		printf_("ERROR: SDL2 Window's Renderer couldn't be initialized\n", PRINTF_VITAL);
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

tile_t::tile_t() : array(this, ARRAY_SETTING_SEND){
	for(int c = 0;c < TILE_ANIMATION_SIZE;c++){
		for(int i = 0;i < TILE_IMAGE_SIZE;i++){
			image[c][i] = 0;
			array.int_array.push_back(std::make_pair(&image[c][i], "tile image " + std::to_string(c) + " " + std::to_string(i) + " "));
		}
	}
	current_animation_entry = 0;
	array.int_array.push_back(std::make_pair(&current_animation_entry, "current animation entry"));
}

image_t *tile_t::get_image(uint_ c, uint_ a){
	if(c > TILE_ANIMATION_SIZE || a > TILE_IMAGE_SIZE){
		printf_("ERROR: get_image: requested out of bounds data\n", PRINTF_ERROR);
		assert(false);
		return nullptr;
	}
	return (image_t*)find_pointer(image[c][a]);
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

image_t *tile_t::get_current_image(){
	double int_part = 0, get_time_ = get_time();
	uint_ entry = (modf(get_time_, &int_part)*TILE_IMAGE_SIZE)-.5;
	return get_image(get_current_animation_entry(), entry); // have to round down since 128 isn't valid
}

void tile_t::set_image_id(uint_ tmp, uint_ entry, array_id_t id){
	if(entry > TILE_IMAGE_SIZE){
		printf_("ERROR: entry > TILE_IMAGE_SIZE\n", PRINTF_ERROR);
		entry = 0;
	}
	image[tmp][entry] = id;
}

tile_t::~tile_t(){
}

image_t::image_t(std::string filename_) : array(this, ARRAY_SETTING_SEND){
	filename = filename_;
	array.string_array.push_back(std::make_pair(&filename, "filename"));
	array.data_type = "image_t";
	surface = IMG_Load(filename_.c_str());
	if(surface == nullptr){
		printf_("ERROR: Couldn't load '" + filename_ + "' as an SDL_Surface.\n", PRINTF_ERROR);
	}else{
		printf_("STATUS: Loaded '" + filename_ + "'\n", PRINTF_STATUS);
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
	tile_t *tmp_tile = (tile_t*)find_pointer(tmp->tile_id);
	if(tmp_tile == nullptr){
		printf_("ERROR: Couldn't find the tmp_tile for screen_t::write_to_screen\n", PRINTF_ERROR);
		return;
	}
	image_t *tmp_image = tmp_tile->get_current_image();
	if(tmp_image == nullptr){
		printf_("ERROR: tmp_image == nullptr\n", PRINTF_ERROR);
		return;
	}
	bool no_surface = false;
	SDL_Surface *tmp_surface = tmp_image->get_surface();
	if(tmp_surface == nullptr){
		printf_("ERROR: tmp_surface == nullptr\n", PRINTF_ERROR);
		no_surface = true;
	}
	SDL_Texture *tmp_texture = nullptr;
	if(no_surface && no_tile_tile != nullptr){
		image_t *tmp_image = no_tile_tile->get_current_image();
		if(tmp_image == nullptr){
			printf_("ERROR: tmp_image == nullptr\n", PRINTF_ERROR);
			return;
		}
		SDL_Surface *no_tile_surface = tmp_image->get_surface();
		if(no_tile_surface == nullptr){
			printf_("ERROR: no_tile_surface == nullptr\n", PRINTF_ERROR);
			return;
		}
		tmp_texture = SDL_CreateTextureFromSurface(renderer, no_tile_surface);
	}else{
		tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	}
	SDL_RenderCopy(renderer, tmp_texture, NULL, &dest);
	SDL_DestroyTexture(tmp_texture);
}
