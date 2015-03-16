#ifndef RENDER_OPENGL_DEP_H
	#define RENDER_OPENGL_DEP_H
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
	#include "../class/class_main.h"
	class render_opengl_dep_t{
	private:
	public:
		render_data_t *data;
		render_opengl_dep_t(render_data_t*);
		~render_opengl_dep_t();
		void loop();
		void loop_init();
		void loop_render_screen();
		void loop_update();
		void loop_render_buffer();
		void generate_window();
		void destroy_window();
	};
	extern int render_generate_shape();
#endif
