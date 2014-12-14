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
/*
TODO: Unify all of the controls in a file and load that into the program.
*/
#include "input_main.h"
#include "../render/render_main.h"
#include "../util/util_main.h"

char *event_key_to_key(SDL_Keycode);

extern render_t *render;
extern client_t *self;

extern void delete_array(array_t*);

input_buffer_t input_buffer_blank;

void input_t::input_find_key(unsigned int *a, const input_buffer_t *b){
	if(b == nullptr || b == NULL){
		for(*a = 0;*a < INPUT_BUFFER_SIZE;(*a)+=1){
			if(input_buffer[*a] == b){
				return;
			}
		}
	}else{
		for(*a = 0;*a < INPUT_BUFFER_SIZE;(*a)+=1){
			if(input_buffer[*a]->type == b->type){
				if(memcmp(&input_buffer[*a]->int_data, &b->int_data, sizeof(int)*8) == 0){
					return;
				}
			}
		}
	}
	*a = INPUT_BUFFER_SIZE;
}

void input_t::input_update_key(input_buffer_t *b){
	unsigned int a = 0;
	input_find_key(&a,b);
	if(a >= INPUT_BUFFER_SIZE){ // there is no match
		input_find_key(&a,nullptr);
		if(a < INPUT_BUFFER_SIZE){ // there is a match
			*input_buffer[a] = *b;
		}else{
			printf("The input array is full\n");
		}
	}
}

void input_t::input_parse_key_up(SDL_Event a){
	input_buffer_t tmp;
	tmp.type = INPUT_TYPE_KEYBOARD;
	tmp.int_data[INPUT_TYPE_KEYBOARD_KEY] = (int)a.key.keysym.sym;
	//tmp.int_data[INPUT_TYPE_KEYBOARD_CHAR] = (int)SDL_GetKeyName(a.key.keysym.sym[0]);
	unsigned int c;
	input_find_key(&c,&tmp);
	if(c <= INPUT_BUFFER_SIZE){
		delete input_buffer[c];
		input_buffer[c] = nullptr;
	}
}

void input_t::input_parse_key_down(SDL_Event a){
	input_buffer_t tmp;
	tmp.type = INPUT_TYPE_KEYBOARD;
	tmp.int_data[INPUT_TYPE_KEYBOARD_KEY] = (int)a.key.keysym.sym;
	//tmp.int_data[INPUT_TYPE_KEYBOARD_CHAR] = (int)SDL_GetKeyName(a.key.keysym.sym[0]);
	input_update_key(&tmp);
}

input_buffer_t::input_buffer_t(){
	array = new_array();
	array->int_array.push_back(&type);
	for(unsigned int i = 0;i < 8;i++){
		array->int_array.push_back(&int_data[i]);
	}
	type = 0;
	for(unsigned int i = 0;i < 8;i++){
		int_data[i] = 0;
	}
}

input_buffer_t::~input_buffer_t(){
	delete_array(array);
	array = nullptr;
}

void input_t::blank(){
	is_used = false;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		input_buffer[i] = nullptr;
	}
}

input_t::input_t(int argc,char** argv){
	SDL_Init(SDL_INIT_EVENTS);
	input_buffer = nullptr;
	input_buffer = new input_buffer_t*[INPUT_BUFFER_SIZE];
	blank();
	printf("%s\n",argv[0]);
	for(int i = 0;i < argc;i++){
		if((std::string)argv[i] == (std::string)"--input-mouse-sensitivity"){ // add yaw and pitch alongside this
	//		input_rules->mouse_yaw_sens = input_rules.mouse_pitch_sens = (float)atof((char*)argv[i+1]);
		}
	}
}

int input_t::loop(){
	int return_value = 0;
	int available_input = 0;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		if(input_buffer[i] != nullptr){
			if(input_buffer[i]->type != INPUT_TYPE_KEYBOARD){
				delete input_buffer[i];
				input_buffer[i] = nullptr;
			}
		}
	}
	while(SDL_PollEvent(&event)){ // the only input that works without a screen would be SDL_QUIT (Ctrl-C).
		switch(event.type){
		case SDL_MOUSEMOTION:
			input_buffer[available_input] = new input_buffer_t;
			if(term_if_true(input_buffer[available_input] == nullptr,(char*)"input_buffer allocation") == TERMINATE){
				return_value = TERMINATE;
				break;
			}
			input_buffer[available_input]->type = INPUT_TYPE_MOUSE_MOTION;
			input_buffer[available_input]->int_data[INPUT_TYPE_MOUSE_MOTION_X] = event.motion.x;
			input_buffer[available_input]->int_data[INPUT_TYPE_MOUSE_MOTION_Y] = event.motion.y;
			break;
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
	/*
	TODO: Add joystick support and map the buttons to keyboard keys
	*/
	return return_value;
}

bool input_t::query_key(input_buffer_t *buffer,int sdl_key,char key){ // This is used for debugging more than anything (all of the other code would be put into the main function here and not stranded in the classes in the code at random spots).
	bool return_value = false;
	if(sdl_key == -1 && key == '\0' && buffer != nullptr){
		for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
			if(input_buffer[i] != nullptr){
				if(input_buffer[i]->int_data == buffer->int_data){
					return_value = true;
					break;
				}
			}
		}
	}else if(sdl_key != -1){
		if(sdl_key < INPUT_BUFFER_SIZE && input_buffer[sdl_key] != nullptr){
			if(input_buffer[sdl_key]->type == INPUT_TYPE_KEYBOARD){
				if(input_buffer[sdl_key]->int_data[INPUT_TYPE_KEYBOARD_KEY] == sdl_key) return_value = sdl_key;
			}
		}else{
			for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
				if(input_buffer[i] != nullptr){
					if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
						if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_KEY] == sdl_key){
							return_value = true;
							break;
						}
					}
				}
			}
		}
	}else if(key != '\0'){
		for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
			if(input_buffer[i] != nullptr){
				if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
					if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_CHAR] == (int)key){
						return_value = true;
						break;
					}
				}
			}
		}
	} // clean this up a bit
	return return_value;
}

void input_t::close(){
	blank();
	delete[] input_buffer;
	input_buffer = nullptr;
}

void input_settings_t::blank(){
	is_used = false;
}

int input_settings_t::init(){
	int return_value = 0;
	blank();
	is_used = true;
	return return_value;
}
void input_settings_t::close(){
	blank();
}

void input_settings_mouse_t::blank(){
	x_sens = 1;
	y_sens = 1;
	slow_key = 'q';
	slow_multiplier = .5; // slow down the mouse movement by 1/2 when Q is pressed
}
int input_settings_mouse_t::init(){
	int return_value = 0;
	blank();
	return return_value;
}

void input_settings_mouse_t::close(){
	blank();
}
