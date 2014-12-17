#include "c_engine.h"
#include "c_input_engine.h"

extern input_t *input;

void input_mouse_motion_engine(input_buffer_t *a){
	//SDL_WarpMouseInWindow(render->render_screen,(render->rules.x_res/2),(render->rules.y_res/2));
}

void input_mouse_press_engine(input_buffer_t *a){}

void input_mouse_scroll_engine(input_buffer_t *a){}

void input_keyboard_engine(input_buffer_t *a){
	if(a->int_data[INPUT_TYPE_KEYBOARD_KEY] == SDLK_f){
		ms_sleep(5000);
	}
}

void input_init(){
}

void input_engine(){
	input_buffer_t** input_buffer = input->input_buffer;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		if(input_buffer[i] == nullptr){
			goto input_loop_end;
		}
		switch(input_buffer[i]->type){
		case INPUT_TYPE_KEYBOARD:
			input_keyboard_engine(input_buffer[i]);
			break;
		case INPUT_TYPE_MOUSE_MOTION:
			input_mouse_motion_engine(input_buffer[i]);
			break;
		case INPUT_TYPE_MOUSE_PRESS:
			input_mouse_press_engine(input_buffer[i]);
			break;
		case INPUT_TYPE_MOUSE_SCROLL:
			input_mouse_scroll_engine(input_buffer[i]);
			break;
		default:
			break;
		}
		input_loop_end:;
	}
}

void input_close(){}

