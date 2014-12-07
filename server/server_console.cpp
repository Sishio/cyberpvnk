#include "server_console.h"

#define NET_CONSOLE_PORT 50010

extern int argc_;
extern char ** argv_;
extern bool terminate;
extern std::vector<coord_t*> coord;

net_t *console_net;

void console_init(){
	console_net = new net_t;
	console_net->ip = new net_ip_t;
	console_net->ip->init(argc_, argv_);
	console_net->ip->set_receive_port(NET_CONSOLE_PORT);
//	a = new std::thread(net_loop,console_net);
}

static void console_parse_packet(const std::string *a){
	if(*a == "exit" || *a == "quit"){
		terminate = true;
	}
	if(*a == "coord_list"){
		const int coord_size = coord.size();
		for(int i = 0;i < coord_size;i++){
			if(coord[i] != NULL){
				coord[i]->print();
			}
		}
		printf("ran the coord_list\n");
	}
}

void console_engine(){
	std::string data = "";
	if((data = console_net->read()) != ""){
		printf("I GOT A LIVE ON\n");
		console_parse_packet(&data);
	}
	console_net->loop();
}

void console_close(){
	console_net->close();
	delete console_net;
	console_net = NULL;
}
