#ifndef INPUT_MAIN_H
	#define INPUT_MAIN_H
	#include "../util/util_main.h"
	#include "../class/class_main.h"
	#include "../class/class_array.h"
	#if defined(__linux) || defined(OSX)
		#include "SDL2/SDL.h"
		#include "signal.h"
		#define POSIX_SIGNALS 1
	#elif _WIN32
		#include "SDL2.h"
	#endif
	class input_keyboard_map_t{
	public:
		array_t array;
		int_ keyboard_map[1024];
		input_keyboard_map_t();
		~input_keyboard_map_t();
	};
	class input_t{
	private:
		array_id_t keyboard_map_id;
		input_keyboard_map_t *keyboard_map;
		void keyboard_to_signal();
	public:
		input_t(int argc, char** argv, array_id_t _keyboard_map_id = DEFAULT_INT_VALUE);
		~input_t();
		void loop();
		bool query_key(int);
	};
	#define SIGNAL_ARRAY_SIZE (_NSIG+16)
	#define SIGNAL_QUIT_LOOP SIGNAL_ARRAY_SIZE-1
	extern bool check_signal(int signal, bool change_signal = false); // stays independent of the input object
	extern void set_signal(int, bool);
	extern bool pull_signal(int);
	// dummy function is used when this is compilied on a non-POSIX OS
#endif
