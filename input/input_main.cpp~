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

#include "input_main.h"
#include "../render/render_main.h"
#include "../util/util_main.h"

struct key_device_t{
	key_device_t(input_buffer_t tmp):input_buffer(tmp){}
	bool operator()(array_t *tmp){
		if(tmp->data_type == "input_buffer_t"){
			return ((input_buffer_t*)tmp->pointer)->int_data == input_buffer.int_data;
		}else{
			return false;
		}
	}
	input_buffer_t input_buffer;
};

char *event_key_to_key(SDL_Keycode);

extern render_t *render;
extern client_t *self;

#ifdef __linux
void signal_handler(int signal){
	switch(signal){
	case SIGINT:
		printf("Received SIGINT\n");
		if(terminate){
			 exit(0);
		}
		terminate = true;
		break;
	case SIGKILL:
		printf("Received SIGKILL\n");
		exit(0);
		break;
	default:
		break;
	}
}
#endif

void input_t::input_update_key(input_buffer_t *b){
	std::vector<array_t*>::iterator a = std::find_if(array_vector.begin(), array_vector.end(), key_device_t(*b));
	if(a == array_vector.end()){
		input_buffer_t *tmp = new input_buffer_t;
		*tmp = *b;
	}else{
		input_buffer_t *tmp = (input_buffer_t*)((*a)->pointer);
		*tmp = *b;
	}
}

void input_t::input_parse_key_up(SDL_Event a){
	input_buffer_t tmp;
	tmp.type = INPUT_TYPE_KEYBOARD;
	tmp.int_data[INPUT_TYPE_KEYBOARD_KEY] = (int)a.key.keysym.sym;
	//tmp.int_data[INPUT_TYPE_KEYBOARD_CHAR] = (int)SDL_GetKeyName(a.key.keysym.sym[0]);
	auto c = std::find_if(array_vector.begin(), array_vector.end(), key_device_t(tmp));
	if(c != array_vector.end()){
		delete (*c);
	}
}

void input_t::input_parse_key_down(SDL_Event a){
	input_buffer_t tmp;
	tmp.type = INPUT_TYPE_KEYBOARD;
	tmp.int_data[INPUT_TYPE_KEYBOARD_KEY] = (int)a.key.keysym.sym;
	//tmp.int_data[INPUT_TYPE_KEYBOARD_CHAR] = (int)SDL_GetKeyName(a.key.keysym.sym[0]);
	input_update_key(&tmp);
}

void input_t::blank(){
	is_used = false;
}

input_t::input_t(int argc,char** argv){
	SDL_Init(SDL_INIT_EVENTS);
	blank();
}

int input_t::loop(){
	int return_value = 0;
	SDL_PumpEvents();
	SDL_Event event;
	while(SDL_PollEvent(&event)){ // the only input that works without a screen would be SDL_QUIT (Ctrl-C).
		switch(event.type){
		case SDL_KEYUP:
			input_parse_key_up(event);
			break;
		case SDL_KEYDOWN:
			input_parse_key_down(event);
			break;
		case SDL_QUIT:
			printf("Terminating per request via input layer\n");
			return_value = TERMINATE;
			break;
		default:
			//printf("The event you are creating is not supported (SDL ID:%d)\n",event.type);
			break;
		}
	}
	return return_value;
}

bool input_t::query_key(input_buffer_t *buffer,int sdl_key,char key){
	bool return_value = false;
	if(sdl_key != -1){
		buffer = new input_buffer_t;
		buffer->type = INPUT_TYPE_KEYBOARD;
		buffer->int_data[INPUT_TYPE_KEYBOARD_KEY] = sdl_key;
	}
	if(buffer != nullptr){
		return std::find_if(array_vector.begin(), array_vector.end(), key_device_t(*buffer)) != array_vector.end();
	}
	if(sdl_key != -1){
		delete buffer;
		buffer = nullptr;
	}
	return return_value;
}

void input_t::close(){
	blank();
}

void cursor::set_location(unsigned int x, unsigned int y){
	#ifdef USE_SDL
	if(render != nullptr){
		SDL_WarpMouseInWindow(render->render_screen, x, y);
	}
	#endif
}

void cursor::get_location(unsigned int *x, unsigned int *y){
	#ifdef USE_SDL
	int *x_ = (int*)x;
	int *y_ = (int*)y;
	SDL_GetRelativeMouseState(x_, y_);
	#endif
}
