#ifndef SERVER_INPUT_H
	#define SERVER_INPUT_H
	#include "../loop/loop_main.h"
	#include "../input/input_main.h"
	extern input_t *input;
	void input_init();
	void input_close();
	void input_engine();
#endif
