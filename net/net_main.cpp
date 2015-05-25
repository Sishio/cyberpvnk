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
#include "net_main.h"

std::string net_generate_ip_address(std::string prefix){
	std::string data_str;
	if(prefix == "10" || prefix == "192"){
		#ifdef __linux
			system("wget checkip.dyndns.org -o IP.htm");
			std::ifstream in("IP.htm");
			char data[512];
			in.getline(data, 512);
			data_str = data_str.substr(data_str.find_first_of(":"), data_str.find_first_of("</")-2-data_str.find_first_of(":"));
			printf("data_str:%s\n",data_str.c_str());
		#endif
	}else{
		
	}
	return data_str;
}

void net_t::blank(){
}

int_ net_t::init_parse_parameters(int_ argc, char** argv){
	for(int_ i = 0;i < argc;i++){
		if(strcmp(argv[i],(char*)"--net-ip") == 0){
			ip = new net_ip_t;
		}
	}
	return 0;
}

int_ net_t::init_initialize_subsystems(int_ argc, char** argv, int_ c){
	if(ip != nullptr){
		ip->init(argc,argv,c);
	}
	return 0;
}

/*static void net_ip_sent_mt(net_ip_t *ip){
	ip->loop_send_mt();
}

static void net_ip_read_mt(net_ip_t *ip){
	ip->loop_receive_mt();
}*/

net_t::net_t(int_ argc, char** argv, int_ b){
	ip = nullptr;
	blank();
	init_parse_parameters(argc,argv);
	init_initialize_subsystems(argc,argv,b);
	packet_id_count = 0;
}

int_ net_t::loop(){
	if(ip != nullptr){
		ip->loop(); // run this anyways because why not?
	}//else printf_("No networking protocol has been selected. Your only option is --net-ip.\n", PRINTF_STATUS);
	return 0;
}

void net_t::write(std::string data, int_ a, uint_ packet_id){ // the data should be sent to the server regardless
	term_if_true(ip == nullptr,(char*)"ip write when ip == nullptr\n");
	if(packet_id == 0){
		packet_id_count++;
		packet_id = packet_id_count;
	}
	ip->write(data, a, packet_id);
}

std::string net_t::read(std::string search){
	if(ip != nullptr){
		return ip->read(search);
	}
	return "";
}

net_t::~net_t(){
	if(ip != nullptr){
		ip->close();
		delete ip;
		ip = nullptr;
	}
}
