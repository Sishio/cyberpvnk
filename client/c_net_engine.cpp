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

array_id_t host_info_id = 0;
array_id_t self_info_id = 0;
extern array_id_t self_id;

extern int argc_;
extern char **argv_;

net_t *net = nullptr;

static loop_t net_loop_mt;

static void net_execute_function(std::string a){
	if(a == "reset_vector"){
		printf("resetting the vector\n");
		delete_all_data();
	}else{
		printf("unknown net_execute_funciton was received: %s\n", a.c_str());
	}
}

static void net_engine_parse(std::string a){
	if(a.find_first_of(ARRAY_TYPE_SEPERATOR_START) != std::string::npos){
		update_class_data(a, CLASS_DATA_UPDATE_EVERYTHING);
	}else if(a.find_first_of(ARRAY_FUNCTION_START) != std::string::npos){
		net_execute_function(a.substr(a.find_first_of(ARRAY_FUNCTION_START)+1, a.find_first_of(ARRAY_FUNCTION_END)-a.find_first_of(ARRAY_FUNCTION_START)-1));
	}
}

//static net_ip_connection_info_t net_generate_self_info(){
//	net_ip_connection_info_t tmp;
//	tmp.ip = "127.0.0.1";
//	tmp.port = net_client_port;
//	return tmp;
//}

static void net_module_loop(){
	net->loop();
}

static void net_connect(int host_info_id_){
	((net_ip_connection_info_t*)find_pointer(host_info_id_, "net_ip_connection_info_t"))->array.print();
	std::string packet = ((net_ip_connection_info_t*)find_pointer(self_info_id, "net_ip_connection_info_t"))->array.gen_updated_string(INT_MAX) + NET_JOIN;
	net->write(packet, host_info_id_);
	bool connection_established = false;
	while(connection_established == false){
		net->loop();
		printf("Waiting for the reply packet\n");
                std::string connecting_packet;
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
	client_t *client_tmp = (client_t*)find_pointer(self_id, "client_t");
	if(client_tmp == nullptr){
		printf("I don't have a pointer to myself yet, cannot send the server any update packet. This is bad\n");
	}else{
		client_tmp->array.data_lock.lock();
		int what_to_update;
		if(likely(once_per_second == false)){
			client_tmp->array.updated(&what_to_update);
		}else{
			what_to_update = INT_MIN;
		}
		// guarantees that data will be sent. if no data is sent after a while, the server pings us out.
		// terrible architecture, I need to fix it
		std::string data = client_tmp->array.gen_updated_string(what_to_update);
		client_tmp->array.data_lock.unlock();
		net->write(data.c_str(), host_info_id);
	}
}

void net_init(){
	loop_add(&net_loop_mt, "net_module_loop", net_module_loop);
	loop_add(&net_loop_mt, "net_receive_engine", net_receive_engine);
	loop_add(&net_loop_mt, "net_send_engine", net_send_engine);
	loop_add(&loop, "net_engine", net_engine);
	net_ip_connection_info_t *self_info = new net_ip_connection_info_t;
	net_ip_connection_info_t *host_info = new net_ip_connection_info_t;
	self_info->array.data_lock.lock();
	host_info->array.data_lock.lock();
	host_info_id = host_info->array.id;
	self_info_id = self_info->array.id;
	host_info->ip = "127.0.0.1"; // don't delete the host
	self_info->ip = "127.0.0.1"; // the IP address that the server should be sending data to
	host_info->port = NET_SERVER_PORT;
	self_info->port = NET_CLIENT_PORT;
	self_info->array.data_lock.unlock();
	host_info->array.data_lock.unlock();
	bool force_ip = false;
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
				force_ip = true;
				self_info->ip = next_item.c_str();
			}
		}else if(strcmp(argv_[i], (char*)"--net-ip-client-port") == 0){
			if(next_item != ""){
				self_info->port = atoi(next_item.c_str());
			}
		}
		if(next_item == ""){
			printf("There is no next object\n");
		}
	}
	if(force_ip == false){
		self_info->array.data_lock.lock();
		self_info->ip = net_gen_ip_address(host_info->ip);
		self_info->array.data_lock.unlock();
	}
	net = new net_t(argc_, argv_, self_info->array.id);
	net_connect(host_info_id);
}

void net_engine(){
	loop_run(&net_loop_mt, 0); // 0: no multithreading, 1: multithreading
}

void net_close(){
	delete net;
	net = nullptr;
	loop_del(&net_loop_mt, net_module_loop);
	loop_del(&net_loop_mt, net_send_engine);
	loop_del(&net_loop_mt, net_receive_engine);
	loop_del(&loop, net_engine);
}
