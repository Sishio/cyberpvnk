#include "../util/util_main.h"
#include "../class/class_main.h"
#include "../class/class_array.h"
#include "input_main.h"

static bool signal_array[SIGNAL_ARRAY_SIZE]; // +16 for my own signals

static inline int entry_from_scancode(long long int a){
	return a;
}

void set_signal(int signal, bool value){
	if(unlikely(signal > SIGNAL_ARRAY_SIZE || signal < 0)){
		printf_("ERROR: signal is out of bounds\n", PRINTF_ERROR);
		return;
	}
	signal_array[signal-1] = value;
}

bool pull_signal(int signal){
	return signal_array[signal-1];
}

void signal_function(int signal){
	if(pull_signal(SIGTERM) && signal == SIGTERM){
		printf("Halting the program because of a hang in the loop\n");
		assert(false);
	}
	set_signal(signal, true);
}

bool check_signal(int signal, bool change_signal){
	const bool return_value = pull_signal(signal);
	if(change_signal == true) set_signal(signal, false);
	return return_value;
}

void input_t::keyboard_to_signal(){
	const bool alt_key = query_key(SDL_SCANCODE_LALT) || query_key(SDL_SCANCODE_RALT);
	if(unlikely(alt_key)){
		if(query_key(SDL_SCANCODE_1)){
			set_signal(SIGNAL_QUIT_LOOP, true);
		}else{
			set_signal(SIGNAL_QUIT_LOOP, false);
		}
	}
}

input_keyboard_map_t::input_keyboard_map_t() : array(this, "input_keyboard_map_t", ARRAY_SETTING_IMMUNITY){
	array.data_lock.lock();
	for(uint_ i = 0;i < KEYBOARD_MAP_SIZE;i++){
		keyboard_map[i] = 0; // flipping bits to save space?
		array.int_array.push_back(std::make_pair(&keyboard_map[i], "keyboard map entry " + std::to_string(i)));
	}
	array.data_lock.unlock();
}

input_keyboard_map_t::~input_keyboard_map_t(){
}

input_t::input_t(int argc, char** argv, array_id_t _keyboard_map_id) : array(this, "input_t", ARRAY_SETTING_IMMUNITY){
	nsig = _NSIG;
	array.int_array.push_back(std::make_pair(&keyboard_map_id, "keyboard map ID"));
	array.int_array.push_back(std::make_pair(&nsig, "_NSIG macro"));
	for(int i = 0;i < 32;i++){
		signal(i, signal_function);
	}
	array.reset_values();
	keyboard_map_id = _keyboard_map_id;
	signal(SIGKILL, signal_function);
	signal(SIGTERM, signal_function);
	signal(SIGINT, signal_function);
}

input_t::~input_t(){
}

void input_t::loop(){
	SDL_PumpEvents();
	SDL_Event event;
	input_keyboard_map_t *keyboard_map = nullptr;
	try{
		keyboard_map = (input_keyboard_map_t*)find_pointer(keyboard_map_id, "input_keyboard_map_t");
		throw_if_nullptr(keyboard_map);
	}catch(std::logic_error &e){
		return;
	}
	keyboard_map->array.data_lock.lock();
	while(SDL_PollEvent(&event)){
		switch(event.type){
		case SDL_KEYUP:
			printf_("DEBUG: SDL_KEYUP was received\n", PRINTF_DEBUG);
			keyboard_map->keyboard_map[entry_from_scancode(event.key.keysym.scancode)] = false;
			break;
		case SDL_KEYDOWN:
			printf_("DEBUG: SDL_KEYDOWN was received\n", PRINTF_DEBUG);
			keyboard_map->keyboard_map[entry_from_scancode(event.key.keysym.scancode)] = true;
			break;
		default:
			// not supported yet, but this should be enough for simple control
			break;
		}
	}
	keyboard_to_signal();
	keyboard_map->array.data_lock.unlock();
}

bool input_t::query_key(int scancode){
	bool return_value = false;
	if(likely(scancode > 0 && scancode < KEYBOARD_MAP_SIZE)){
		try{
			input_keyboard_map_t *keyboard_map = (input_keyboard_map_t*)find_pointer(keyboard_map_id);
			throw_if_nullptr(keyboard_map);
			return_value = keyboard_map->keyboard_map[scancode];
		}catch(std::logic_error &e){}
	}else{
		printf_("ERROR: The scancode is outside of the keyboard bounds. You might be trying to acess some special variable (mouse control, joystick control, etc.)\n", PRINTF_ERROR);
		return_value = false;
	}
	return return_value;
}
