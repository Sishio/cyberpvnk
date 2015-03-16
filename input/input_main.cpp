#include "input_main.h"

static bool signal_array[SIGNAL_ARRAY_SIZE]; // +16 for my own signals

static inline int entry_from_scancode(long long int a){
	return a;
}

static inline void set_signal(int signal, bool value){
	signal_array[signal-1] = value;
}

static inline bool pull_signal(int signal){
	return signal_array[signal-1];
}

static inline void signal_function(int signal){
	set_signal(signal, true);
}

bool check_signal(int signal){
	const bool return_value = pull_signal(signal);
	set_signal(signal, false);
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

input_t::input_t(int argc, char** argv){
	for(int i = 0;i < 32;i++){
		signal(i, signal_function);
	}
	//signal(SIGKILL, signal_function);
	//signal(SIGTERM, signal_function);
	//signal(SIGINT, signal_function);
}

input_t::~input_t(){}

void input_t::loop(){
	SDL_PumpEvents();
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
		case SDL_KEYUP:
			keyboard_map.keyboard_map[entry_from_scancode(event.key.keysym.scancode)] = false;
			break;
		case SDL_KEYDOWN:
			keyboard_map.keyboard_map[entry_from_scancode(event.key.keysym.scancode)] = true;
			break;
		default:
			// not supported yet, but this should be enough for simple control
			break;
		}
	}
	keyboard_to_signal();
}

bool input_t::query_key(int scancode){
	bool return_value = false;
	if(likely(scancode > 0 && scancode < 256)){
		return_value = keyboard_map.keyboard_map[scancode];
	}else{
		printf("The scancode is outside of the keyboard bounds\n");
		return_value = false;
	}
	return return_value;
}

input_keyboard_map_t::input_keyboard_map_t() : array(this, true){
	client_id = -1;
	array.int_array.push_back(&client_id);
	for(unsigned long int i = 0;i < INPUT_MAP_SIZE;i++){
		array.int_array.push_back(&keyboard_map[i]);
		keyboard_map[i] = false;
	}
}

input_keyboard_map_t::~input_keyboard_map_t(){
}
