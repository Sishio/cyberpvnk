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
#include "c_main.h"
#include "signal.h"

std::vector<coord_t*> coord;
std::vector<model_t*> model;
std::vector<client_t*> client;

client_t *self = nullptr;

render_t *render = nullptr;
input_t *input = nullptr;
net_t *net = nullptr;

static int argc_;
static char **argv_;

unsigned long int tick = 0;
bool terminate = false;

static void init(){
	printf("Allocating & initializing self\n");
	self = new client_t();
	printf("Allocating & initializing render\n");
	render = new render_t(argc_,argv_);
	printf("Allocating & initializing input\n");
	input = new input_t(argc_,argv_);
	printf("Allocating & initializing net\n");
	net = new net_t(argc_,argv_);
}

static void close(){
	render->close();
	delete render;
	render = nullptr;
	input->close();
	delete input;
	input = nullptr;
	net->close();
	delete net;
	net = nullptr;
	self->close();
	delete self;
	self = nullptr;
}

static void engine_loop(){
	if(net != nullptr) net_engine();
	if(input != nullptr) input_engine();
	if(render != nullptr) render_engine();
}

static void module_loop(){
	if(net != nullptr) net->loop();
	if(input != nullptr && input->loop() == TERMINATE) terminate = true;
	if(render != nullptr) render->loop();
}

static void loop(){
	engine_loop();
	module_loop();
}

#ifdef __linux
void signal_handler(int signal){
	if(signal == SIGINT){
		printf("Received SIGINT\n");
		terminate = true;
	}
}
#endif

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	printf("Initializing the modules and other data\n");
	init();
	#ifdef __linux
	printf("Setting up the signal handler\n");
	signal(SIGINT,signal_handler);
	#endif
	while(terminate == false){
		loop();
		tick++;
	}
	close();
}