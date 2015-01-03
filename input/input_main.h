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
#ifndef C_INPUT_H
	#define C_INPUT_H
	#define USE_SDL 1
	#include "../main.h"
	#include "signal.h"
	#include "cstdio"
	#include "string"
	#include "cassert"
	#include "../class/class_main.h"
	#ifdef __linux
		#include "SDL2/SDL.h"
	#elif _WIN32
		#include "SDL2.h"
	#endif
	extern bool terminate;
	// add support for joysticks and cool things later on
	namespace cursor{
		void set_location(unsigned int, unsigned int);
		void get_location(unsigned int*, unsigned int*);
	}
	class input_t{
		private:
			void input_find_key(long int*, const input_buffer_t*);
			void input_update_key(input_buffer_t*);
			void input_parse_key_up(SDL_Event);
			void input_parse_key_down(SDL_Event);
			void input_parse_mouse_motion(SDL_Event);
		public:
			bool is_used;
			SDL_Event event;
			void blank();
			input_t(int,char**);
			bool query_key(input_buffer_t* buffer = NULL,int sdl_key = 0,char key = '\0');
			int loop();
			void close();
	};
	#ifdef __linux
	extern void signal_handler(int);
	#endif
	#include "../class/class_main.h"
#endif
