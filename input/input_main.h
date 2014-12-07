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
	struct input_buffer_t{
		int type;
		int *int_data; // I cannot think of a need for floats as of now (we are only taking in the data, we are NOT processing it)
		int int_data_size;
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
			bool is_used;
			input_buffer_t **input_buffer;
			SDL_Event event;
			void standard_input_check();
		public:
			coord_t *coord;
			void blank();
			int init(int,char**);
			bool query_key(input_buffer_t* buffer = NULL,int sdl_key = 0,char key = '\0');
			int loop();
			void close();
	};
	#include "../class/class_main.h"
	extern input_buffer_t input_buffer_blank;
#endif
