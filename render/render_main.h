#ifdef RENDER_MAIN_H
#define RENDER_MAIN_H
#include "cstdio"
#ifdef __linux
#include "SDL2/SDL2.h"
#include "SDL2/SDL_opengl.h"
#else
#include "SDL.h"
#include "SDL_opengl.h"
#endif
struct image_t{
private:
	std::string filename;
public:	
	image_t(std::string);
	~image_t();
	void load();
	std::string get_filename();
	std::string get_image_string();
};
struct tile_t{
private:
	array_id_t image_id[128];
	bool render;
public:
	tile_t(array_id_t);
	~tile_t();
	image_t *get_image(int_); // int = image in animation
	array_id_t get_image_id(int_);
};

struct screen_t{
private:
	array_t array;
	int_ x_res, y_res;
	std::string title;
public:
	
};

class render_t{
 private:
	std::vector<array_id_t> screen;
 public:
	render_t(int_, char**);
	~render_t();
	void loop();
};
#endif
