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
#include "c_net_engine.h"

extern bool once_per_second;
extern bool terminate;

int host_info_id = 0;
int self_info_id = 0;
extern int self_id;

extern int argc_;
extern char **argv_;

net_t *net = nullptr;

static loop_t net_loop_mt;

static void net_engine_parse(std::string a){
	if(a.find_first_of(ARRAY_TYPE_SEPERATOR_START) != std::string::npos){
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

void net_module_loop(){
	net->loop();
}

static void net_connect(int host_info_id){
	assert(find_pointer(self_id) != nullptr);
	client_t *tmp_client = (client_t*)find_pointer(self_id);
	assert(tmp_client != nullptr);
	net_ip_connection_info_t net_conn_info;
	net_conn_info.ip = "127.0.0.1";
	net_conn_info.port = NET_CLIENT_PORT;
	std::string packet = net_conn_info.array.gen_updated_string(INT_MAX);
	packet += NET_JOIN;
	printf("Sending packet '%s'\n",packet.c_str());
	net->write(packet, host_info_id);
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
	client_t *client_tmp = (client_t*)find_pointer(self_id);
	if(client_tmp == nullptr){
		printf("I don't have a pointer to myself yet, cannot send the server any update packet. This is bad\n");
		return;
	}
	int what_to_update = INT_MIN;
	std::string data = client_tmp->array.gen_updated_string(what_to_update);
	net->write(data.c_str(), host_info_id);
}

void net_init(){
	loop_add(&net_loop_mt, net_module_loop);
	loop_add(&net_loop_mt, net_receive_engine);
	loop_add(&net_loop_mt, net_send_engine);
	loop_add(&loop, net_engine);
	net_ip_connection_info_t *self_info = new net_ip_connection_info_t;
	net_ip_connection_info_t *host_info = new net_ip_connection_info_t;
	host_info_id = host_info->array.id;
	self_info_id = self_info->array.id;
	host_info->ip = "127.0.0.1";
	self_info->ip = "127.0.0.1";
	host_info->port = NET_SERVER_PORT;
	self_info->port = NET_CLIENT_PORT;
	net = new net_t(argc_, argv_, self_info->array.id);
	for(int i = 0;i < argc_;i++){
		std::string next_item;
		if(i+1 < argc_){
			next_item = argv_[i+1];
		}else{
			next_item = "";
		}
		if(strcmp(argv_[i], (char*)"--net-ip-server-ip") == 0){
			host_info->ip = (std::string)argv_[i+1];
		}else if(strcmp(argv_[i], (char*)"--net-ip-server-port") == 0){
			host_info->port = atoi((char*)argv_[i+1]);
		}else if(strcmp(argv_[i], (char*)"--net-ip-client-ip") == 0){
			if(next_item != ""){
				self_info->ip = next_item.c_str();
			}
		}else if(strcmp(argv_[i], (char*)"--net-ip-client-port") == 0){
			if(next_item != ""){
				self_info->port = atoi(next_item.c_str());
			}
		}
	}
	net_connect(host_info_id);
}

void net_engine(){
	loop_run(&net_loop_mt, 1); // the first bit is the only one that is changed
}

void net_close(){
	loop_del(&net_loop_mt, net_module_loop);
	loop_del(&net_loop_mt, net_send_engine);
	loop_del(&net_loop_mt, net_receive_engine);
	loop_del(&loop, net_engine);
}
