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

extern int_ argc_;
extern char **argv_;

net_t *net = nullptr;

static loop_t net_loop_mt;

static void net_module_loop(){
	net->loop();
}

static void net_connect(){
	((net_ip_connection_info_t*)find_pointer(host_info_id, "net_ip_connection_info_t"))->array.print();
	std::string packet = ((net_ip_connection_info_t*)find_pointer(self_info_id, "net_ip_connection_info_t"))->array.gen_updated_string(INT_MAX) + NET_JOIN;
	net->write(packet, host_info_id);
	bool connection_established = false;
	long double start_time = get_time();
	while(connection_established == false && infinite_loop()){
		net->loop();
		std::string connecting_packet;
		if((connecting_packet = net->read(NET_JOIN)) != ""){
			printf("Received '%s' from the server\n", connecting_packet.c_str());
			connecting_packet.erase(connecting_packet.begin()+connecting_packet.find_first_of(NET_JOIN));
			self_id = atoi(connecting_packet.c_str());
			connection_established = true;
		}
	}
	if(connection_established == false){
		printf("The server didn't respond in time (5 seconds). I am shutting down the entire network engine to save resources.\n");
		net_close();
	}
}

static void net_receive_engine(){
	std::string a;
	for(int_ i = 0;i < 512;i++){
		if((a = net->read()) != ""){
			update_class_data(a, CLASS_DATA_UPDATE_EVERYTHING);
		}else{
			break;
		}
	}
}

static void net_send_engine(){
	client_t *client_tmp = (client_t*)find_pointer(self_id, "client_t");
	if(likely(client_tmp != nullptr)){
		client_tmp->array.data_lock.lock();
		int_ what_to_update;
		if(likely(once_per_second == false)){
			client_tmp->array.updated(&what_to_update);
		}else{
			what_to_update = INT_MIN;
		}
		std::string data = client_tmp->array.gen_updated_string(what_to_update);
		client_tmp->array.data_lock.unlock();
		net->write(data.c_str(), host_info_id);
	}
}

static void net_init_loop(){
	loop_add(&net_loop_mt, "net_module_loop", net_module_loop);
	loop_add(&net_loop_mt, "net_receive_engine", net_receive_engine);
	loop_add(&net_loop_mt, "net_send_engine", net_send_engine);
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
	for(int_ i = 0;i < argc_;i++){
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
	net = new net_t(argc_, argv_, ((net_ip_connection_info_t*)find_pointer(self_info_id, "net_ip_connection_info_t"))->array.id);
	net_connect();
}

void net_init(){
	loop_add(&loop, "net_engine", net_engine);
}

void net_engine(){
	int_ loop_settings = 0;
	SET_BIT(loop_settings, LOOP_CODE_NEVEREND_MT, 1);
	if(unlikely(net == nullptr)){
		/*
		  If there is some loop inside of the initialization
		  code that needs to be stopped, there is a way to
		  access it with the input library (either with the
		  window input or signals). This also makes it easier
		  for one initializer to use another's resources (although
		  this should never be the case).
		 */
		net_init_loop();
	}
	loop_run(&net_loop_mt);
}

void net_close(){
	if(likely(net != nullptr)){
		delete net;
		net = nullptr;
	}
	loop_del(&net_loop_mt, net_module_loop);
	loop_del(&net_loop_mt, net_send_engine);
	loop_del(&net_loop_mt, net_receive_engine);
	loop_del(&loop, net_engine);
}
