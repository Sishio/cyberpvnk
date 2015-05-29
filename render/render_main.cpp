#include "render_main.h"

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

render_t::render_t(int_ argc, char** argv){
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	load_all_images();
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf_("ERROR: SDL2 Video couldn't be initialized", PRINTF_VITAL);
		return;
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
	//if(SDL_UpdateWindowSurface(tmp_screen->screen) < 0){
	//	printf_("ERROR: SDL2: '" + (std::string)SDL_GetError() + "'\n", PRINTF_ERROR);
	//}
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

screen_t::screen_t() : array(this, false){
	array.int_array.push_back(&x_res);
	array.int_array.push_back(&y_res);
	array.string_array.push_back(&title);
	screen = nullptr;
	screen_surface = nullptr;
	renderer = nullptr;
	x_res = 640;
	y_res = 480;
	title = "Title";
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
		printf_("ERROR: SDL2 Window's Renderer couldn't be initialized\n", PRINTF_ERROR);
	}
}

void screen_t::del_screen(){
	if(screen_surface != nullptr){
		SDL_FreeSurface(screen_surface);
		screen_surface = nullptr;
	}
	if(screen != nullptr){
		SDL_DestroyWindow(screen);
		screen_surface = nullptr;
	}
}

tile_t::tile_t() : array(this, true){
	for(int i = 0;i < 128;i++){
		image[i] = 0;
		array.int_array.push_back(&image[i]);
	}
}

image_t *tile_t::get_image(uint_ a){
	return (image_t*)find_pointer(image[a]);
}

image_t *tile_t::get_current_image(){
	double int_part = 0, get_time_ = get_time();
	return get_image((uint_)(modf(get_time_, &int_part)*TILE_IMAGE_SIZE)-.5); // have to round down since 128 isn't valid
}

void tile_t::set_image_id(uint_ entry, array_id_t id){
	image[entry] = id;
}

tile_t::~tile_t(){
}

image_t::image_t(std::string filename_) : array(this, true){
	filename = filename_;
	array.string_array.push_back(&filename);
	array.data_type = "image_t";
	surface = IMG_Load(filename_.c_str());
	if(surface == nullptr){
		printf_("ERROR: Couldn't load '" + filename_ + "' as a SDL_Surface.\n", PRINTF_ERROR);
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
	dest.x = tmp->x;
	dest.y = tmp->y;
	dest.w = 16; // width and height should be fixed at 16, but define this inside the tile_t anyways
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
	SDL_Surface *tmp_surface = tmp_image->get_surface();
	if(tmp_surface == nullptr){
		printf_("ERROR: tmp_surface == nullptr\n", PRINTF_ERROR);
		return;
	}
	SDL_Texture *tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	//SDL_BlitSurface(tmp_surface, NULL, screen_surface, &dest);
	SDL_RenderCopy(renderer, tmp_texture, NULL, &dest);
	SDL_DestroyTexture(tmp_texture); // cheap
}
