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
	#include "../main.h"
	#include "cstdio"
	#include "string"
	#include "cassert"
	#include "../class/class_main.h"
	#ifdef __linux
		#include "SDL2/SDL.h"
	#elif _WIN32
		#include "SDL2.h"
	#endif
	#define INPUT_BUFFER_SIZE 1024 // there should be no need to increase the size
	#define INPUT_TYPE_KEYBOARD 0
		#define INPUT_TYPE_KEYBOARD_INT_SIZE 2
		#define INPUT_TYPE_KEYBOARD_KEY 0 // SDLK_*
		#define INPUT_TYPE_KEYBOARD_CHAR 1
	#define INPUT_TYPE_MOUSE_MOTION 1
		#define INPUT_TYPE_MOUSE_MOTION_INT_SIZE 2
		#define INPUT_TYPE_MOUSE_MOTION_X 0
		#define INPUT_TYPE_MOUSE_MOTION_Y 1
	#define INPUT_TYPE_MOUSE_PRESS 2 // type
		#define INPUT_TYPE_MOUSE_PRESS_INT_SIZE 1
		#define INPUT_TYPE_MOUSE_PRESS_BUTTON 0; // ID entry
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_LEFT 0 // values
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_MIDDLE 1
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_RIGHT 2
	#define INPUT_TYPE_MOUSE_SCROLL 3
		#define INPUT_TYPE_MOUSE_SCROLL_UP 0
		#define INPUT_TYPE_MOUSE_SCROLL_DOWN 1
	// add support for joysticks and cool things later on
	class input_buffer_t{
	public:
		input_buffer_t();
		~input_buffer_t();
		array_t *array;
		int type;
		int int_data[8];
	};
	class input_settings_mouse_t{
	public:
		float x_sens;
		float y_sens;
		char slow_key; // enable a key to slow down the sensitivity
		float slow_multiplier;
		void blank();
		int init();
		void close();
	};
	class input_settings_t{
	public:
		input_settings_mouse_t mouse;
		input_settings_mouse_t mouse_default;
		bool is_used;
		void blank();
		int init();
		void close();
	};
	class input_t{
		private:
			void input_find_key(unsigned int*, const input_buffer_t*);
			void input_update_key(input_buffer_t*);
			void input_parse_key_up(SDL_Event);
			void input_parse_key_down(SDL_Event);
		public:
			bool is_used;
			input_buffer_t **input_buffer;
			SDL_Event event;
			void blank();
			input_t(int,char**);
			bool query_key(input_buffer_t* buffer = NULL,int sdl_key = 0,char key = '\0');
			int loop();
			void close();
	};
	#include "../class/class_main.h"
	extern input_buffer_t input_buffer_blank;
#endif
