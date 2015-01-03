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

#include "c_engine.h"
#include "c_net_engine.h"

extern bool once_per_second;
extern bool terminate;

extern int host_info_id;
extern int self_id;

static void net_engine_parse(std::string a){
	if(a.find_first_of(ARRAY_ITEM_SEPERATOR_START) != std::string::npos){
		update_class_data(a, CLASS_DATA_UPDATE_EVERYTHING);
	}else{
		printf("Unknown data was received:%s\n", a.c_str());
	}
}

//static net_ip_connection_info_t net_generate_self_info(){
//	net_ip_connection_info_t tmp;
//	tmp.ip = "127.0.0.1";
//	tmp.port = net_client_port;
//	return tmp;
//}

static void net_connect(int host_info_id){
	assert(find_client_pointer(self_id) != nullptr);
	client_t *tmp_client = find_client_pointer(self_id);
	assert(tmp_client != nullptr);
	net_ip_connection_info_t *self_info = find_net_ip_connection_info_pointer(tmp_client->connection_info_id);
	assert(self_info != nullptr);
	self_info->ip = "127.0.0.1";
	self_info->port = NET_CLIENT_PORT;
	std::string packet = self_info->array.gen_string();
	packet += NET_JOIN;
	printf("Sending packet '%s'\n",packet.c_str());
	net->write(packet, host_info_id, gen_rand());
	std::string connecting_packet;
	bool connection_established = false;
	while(connection_established == false){
		net->loop();
		if((connecting_packet = net->read(NET_JOIN)) != ""){
			printf("Received '%s' from the server\n", connecting_packet.c_str());
			connecting_packet.erase(connecting_packet.begin()+connecting_packet.find_first_of(NET_JOIN));
			self_id = atoi(connecting_packet.c_str());
			connection_established = true;
		}
	}
	self_info = nullptr;
}

void net_init(){
	host_info_id = new_net_ip_connection_info()->array.id;
	find_net_ip_connection_info_pointer(host_info_id)->ip = "127.0.0.1";
	find_net_ip_connection_info_pointer(host_info_id)->port = NET_SERVER_PORT;
	for(int i = 0;i < argc_;i++){
		if(strcmp(argv_[i], (char*)"--net-ip-server-ip") == 0){
			find_net_ip_connection_info_pointer(host_info_id)->ip = (std::string)argv_[i+1];
		}else if(strcmp(argv_[i], (char*)"--net-ip-server-port") == 0){
			find_net_ip_connection_info_pointer(host_info_id)->port = atoi((char*)argv_[i+1]);
		}
	}
	net_connect(host_info_id);
}

static void net_receive_engine(){
	std::string a;
	for(unsigned long int i = 0;i < 512;i++){
		if((a = net->read()) != ""){
			net_engine_parse(a);
		}else{
			break;
		}
	}
}

static void net_send_engine(){
}

void net_engine(){
	net->loop();
	net_receive_engine();
	net_send_engine();
}

void net_close(){
	assert(terminate == true);
}
