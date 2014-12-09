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
#ifndef C_RENDER_H
	#define C_RENDER_H
	#define GL_GLEXT_PROTOTYPES
	#include "render_obj_loader.h"
	#include "../util/util_main.h"
	#include "../math/math_main.h"
	#include "../class/class_main.h"
	#include "cstdio"
	#include "cstdlib"
	#include "cassert"
	#include "cmath"
	#include "SDL2/SDL_opengl.h"
	#ifdef __linux
		#include "SDL2/SDL.h"
	#elif _WIN32
		#include "SDL2.h"
	#endif
	#include "GL/gl.h"
	#include "GL/glu.h"
	#include "../util/util_main.h"
	#define RENDER_BUFFER_SIZE 2048 // +1 is there since optimizations levels tend to slow down the code if it is a power of 2
	#define MODEL_BUFFER_SIZE RENDER_BUFFER_SIZE
	#define RENDER_DIMENSION_3D 0
	#define RENDER_DIMENSION_2D 1
	#define MODEL_TYPE_CLIENT 0 // used to calibrate the camera before rendering the scene
	#define MODEL_TYPE_OBJECT 1 // everything else
	#define RENDER_TYPE_CLIENT MODEL_TYPE_CLIENT
	#define RENDER_TYPE_OBJECT MODEL_TYPE_OBJECT
	#define RENDER_TYPE_MACRO MODEL_TYPE_MACRO
	#define RENDER_GENERATE_SHAPE_PLANE 1
	#define RENDER_GENERATE_SHAPE_GRID 0
	#define RENDER_OPENGL_POWER_UNSUPPORTED 0
	#define RENDER_OPENGL_POWER_DEPRECATED 1
	#define RENDER_OPENGL_POWER_USED 2
	struct render_opengl_powers_t{
		char gl_begin_end; // deprecated with OpenGL 2
		char vertex_buffer_objects;
	};
	class render_rules_t{
	public:
		void blank();
		void init();
		void close();
		bool fullscreen;
		bool vsync;
		unsigned int x_res;
		unsigned int y_res;
		unsigned int x_offset;
		unsigned int y_offset;
		float fov;
		float near_fov;
		float far_fov;
		char* window_title;
		bool enable_hud;
		bool enable_view;
		bool enable_grid;
		render_opengl_powers_t opengl_powers;
	};
	struct render_buffer_t{
		coord_t *coord;
		model_t *model;
		bool delete_on_render;
	};
	class render_t{
	private:
		SDL_GLContext render_context;
		render_buffer_t **render_buffer;
		void init_generate_window();
		void init_subsystems();
		void init_parse_parameters(int,char**);
		void init_configure_buffer();
		void init_load_models();
		void loop_init();
		void loop_update();
		void loop_render_buffer();
		void loop_render_screen();
	public:
		coord_t *coord;
		SDL_Window *render_screen;
		render_rules_t rules;
		void blank();
		render_t(int,char**);
		int loop();
		int write_to_buffer(render_buffer_t*);
		void close();
	};
	#include "../class/class_main.h"
	extern render_rules_t *render_rules_blank;
	extern render_buffer_t *render_buffer_blank;
	extern int render_generate_shape(int);
#endif
