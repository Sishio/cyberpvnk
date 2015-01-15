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

void net_write_array_vector(std::vector<std::vector<std::string>> a, int b, net_t *net){
	const unsigned long int packet_id = gen_rand();
	const unsigned long int a_size = a.size();
	for(unsigned long int i = 0;i < a_size;i++){
		const unsigned long int a_i_size = a[i].size();
		for(unsigned long int n = 0;n < a_i_size;n++){
			net->write(a[i][n], b, packet_id);
		}
	}
}

void net_loop(net_t* a){
	while(true){
		a->loop();
		ms_sleep(1);
	}
}

void net_t::blank(){
}

int net_t::init_parse_parameters(int argc, char** argv){
	for(int i = 0;i < argc;i++){
		if(strcmp(argv[i],(char*)"--net-ip") == 0){
			ip = new net_ip_t;
		}
	}
	return 0;
}

int net_t::init_initialize_subsystems(int argc, char** argv, int c){
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

net_t::net_t(int argc, char** argv, int b){
	ip = nullptr;
	blank();
	init_parse_parameters(argc,argv);
	init_initialize_subsystems(argc,argv,b);
}

int net_t::loop(){
	if(ip != nullptr){
		ip->loop(); // run this anyways because why not?
	}else printf("No networking protocol has been selected. Your only option is --net-ip.\n");
	return 0;
}

void net_t::write(std::string data, int a, unsigned long int packet_id){ // the data should be sent to the server regardless
	term_if_true(ip == nullptr,(char*)"ip write when ip == nullptr\n");
	if(packet_id == 0){
		packet_id = packet_id_count;
		packet_id_count++;
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
