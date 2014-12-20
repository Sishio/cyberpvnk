/*
Simple networking library that can do a lot of things.
This is meant as an end all be all networking library for this project.
There are a few limitations:
	Since I am now using std::string for the data, the incoming data must be ASCII. Non-compliant data would probably get corrupt in the casting.
	There is no TCP support, only UDP. This means sending large pieces of data (greater than 484 bytes) is not a good idea with IP. Serial should have no problem with that when the time comes to make that side.

*/
#include "net_main.h"

void net_loop(net_t* a){
	while(true){
		a->loop();
		ms_sleep(1);
	}
}

void net_t::blank(){}

int net_t::init_parse_parameters(int argc, char** argv){
	for(int i = 0;i < argc;i++){
		if(strcmp(argv[i],(char*)"--net-ip") == 0){
			ip = new net_ip_t;
		}
	}
	return 0;
}

int net_t::init_initialize_subsystems(int argc, char** argv){
	if(ip != nullptr){
		ip->init(argc,argv);
	}
	return 0;
}

net_t::net_t(int argc, char** argv){
	ip = nullptr;
	blank();
	init_parse_parameters(argc,argv);
	init_initialize_subsystems(argc,argv);
}

int net_t::loop(){
	if(ip != nullptr){
		ip->loop();
	}else printf("No networking protocol has been selected. Your only option is --net-ip.\n");
	return 0;
}
int net_t::write(std::string data, net_ip_connection_info_t a){ // the data should be sent to the server regardless
	term_if_true(ip == nullptr,(char*)"ip write when ip == nullptr\n");
	ip->write(data,a);
	return 0;
}

std::string net_t::read(){
	if(ip != nullptr){
		return ip->read();
	}
	return "";
}

void net_t::close(){
	if(ip != nullptr){
		ip->close();
		delete ip;
		ip = nullptr;
	}
}
