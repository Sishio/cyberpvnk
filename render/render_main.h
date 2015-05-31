#ifndef RENDER_MAIN_H
#define RENDER_MAIN_H
#include "../main.h"
#include "cstdio"
#ifdef __linux
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_image.h"
#else
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#endif
#include "string"
#include "iostream"
#define TILE_IMAGE_SIZE 128
struct image_t{
private:
	std::string filename;
	SDL_Surface *surface;
public:
	void free_ram();
	array_t array;
	image_t(std::string);
	~image_t();
	void load();
	void write_to_screen(int_, int_);
	std::string get_filename();
	std::string get_image_string();
	SDL_Surface *get_surface(){return surface;}
};
#define TILE_ANIMATION_SIZE 32
struct tile_t{
private:
	array_id_t image[TILE_ANIMATION_SIZE][TILE_IMAGE_SIZE];
	bool render;
	int_ current_animation_entry;
public:
	array_t array;
	tile_t();
	~tile_t();
	image_t *get_image(uint_, uint_);
	image_t *get_current_image();
	uint_ get_current_animation_entry();
	void set_current_animation_entry(uint_);
	void set_image_id(uint_, uint_, array_id_t);
};

struct screen_t{
public:
	array_t array;
	int_ x_res, y_res;
	std::string title;
	SDL_Window  *screen;
	SDL_Surface *screen_surface;
	SDL_Renderer *renderer;
	screen_t();
	~screen_t();
	void new_screen();
	void del_screen();
	void write_to_screen(coord_t*);
};

class render_t{
 public:
	array_t array;
	std::vector<array_id_t> screen;
	render_t(int_, char**);
	~render_t();
	void loop();
};
#endif
