#ifndef INPUT_MAIN_H
	#define INPUT_MAIN_H
	#include "../class/class_array.h"
	#ifdef __linux
		#include "SDL2/SDL.h"
		#include "signal.h"
		#define POSIX_SIGNALS 1
	#elif _WIN32
		#include "SDL2.h"
	#endif
	#define INPUT_MAP_SIZE 512
	class input_keyboard_map_t{
	public:
		array_id_t client_id;
		array_t array;
		int keyboard_map[INPUT_MAP_SIZE];
		input_keyboard_map_t();
		~input_keyboard_map_t();
	};
	class input_t{
	private:
		input_keyboard_map_t keyboard_map;
		void keyboard_to_signal();
	public:
		input_t(int, char**);
		~input_t();
		void loop();
		bool query_key(int);
	};
	#define SIGNAL_ARRAY_SIZE (_NSIG+16)
	#define SIGNAL_QUIT_LOOP SIGNAL_ARRAY_SIZE
	extern bool check_signal(int); // stays independent of the input object
	// dummy function is used when this is compilied on a non-POSIX OS
#endif
