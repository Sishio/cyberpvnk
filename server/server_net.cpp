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
#include "server_main.h"
#include "../loop/loop_main.h"
#include "../class/class_main.h"
#include "../class/class_array.h"
#include "server_net.h"

static bool net_init_bool = false;
net_t *net = nullptr;
static loop_t *net_loop_code;

static bool net_check_pingout(client_t *tmp){
	const long double current_time = get_time();
	const long double pingout = 3; // timer is always in seconds
	return tmp->array.last_update + pingout < current_time;
}

static double last_client_net_run = 0;
#define CLIENT_MAX_JOIN_RATE .2

static void net_client_join(std::string data){
	printf_("STATUS: Accepted the client\n", PRINTF_STATUS);
	client_t *client_tmp = new client_t;
	net_ip_connection_info_t *tmp_net_ip = new net_ip_connection_info_t;
	tmp_net_ip->array.parse_string_entry(data);
	client_tmp->connection_info_id = tmp_net_ip->array.id;
	client_tmp->model_id = (new model_t)->array.id;
	client_tmp->coord_id = (new coord_t)->array.id;
	//printf("tmp_net_ip.ip:%s\ttmp_net_ip.port: %d\n", tmp_net_ip->ip.c_str(), tmp_net_ip->port);
	std::string return_packet = NET_JOIN + std::to_string(client_tmp->array.id);
	net->write(return_packet, 0, client_tmp->connection_info_id);
	printf_("STATUS: net_client_join: A new client (ID: " + std::to_string(client_tmp->array.id) + ") has joined. Sending the client their ID\n", PRINTF_STATUS);
}

static void net_send_data();
static void net_pingout();
static void net_read_data();

void net_module(){
	net->loop();
}

void net_init(){
	if(net_init_bool == false){
		net_loop_code = new loop_t;
		net_loop_code->array.name = "net loop code";
		net_ip_connection_info_t *tmp_conn_info = new net_ip_connection_info_t;
		tmp_conn_info->ip = "127.0.0.1";
		tmp_conn_info->port = NET_SERVER_PORT;
		net = new net_t();
		net->set_inbound_info(tmp_conn_info->array.id);
		net_init_bool = true;
		loop_add(net_loop_code, loop_generate_entry(LOOP_CODE_PARTIAL_MT, "net_read_data", net_read_data));
		loop_add(net_loop_code, loop_generate_entry(LOOP_CODE_PARTIAL_MT, "net_pingout", net_pingout));
		loop_add(net_loop_code, loop_generate_entry(LOOP_CODE_PARTIAL_MT, "net_send_data",  net_send_data));
		loop_add(net_loop_code, loop_generate_entry(LOOP_CODE_PARTIAL_MT, "net_module", net_module));
		loop_add(server_loop_code, loop_generate_entry(0, "net_engine",  net_engine));
	}else{
		printf_("STATUS: net_init has already been initalized, not re-running the initializer\n", PRINTF_STATUS);
	}
}

static void net_sync_array_vector(){
	array_sync_t sync;
	sync.read();
	std::string string_ = sync.array.gen_updated_string(~0);
	std::vector<array_id_t> client_vector = all_ids_of_type("client_t");
	const uint_ client_vector_size = client_vector.size();
	for(uint_ i = 0;i < client_vector_size;i++){
		try{
			client_t *tmp_client = (client_t*)find_pointer(client_vector[i]);
			throw_if_nullptr(tmp_client);
			net->write(string_, 0, tmp_client->connection_info_id);
		}catch(const std::logic_error &e){}
	}
}

static void net_send_data(){
	std::vector<array_id_t> client_vector = all_ids_of_type("client_t");
	const uint_ client_vector_size = client_vector.size();
	if(client_vector_size == 0){
		return;
	}
	if(once_per_second){
		net_sync_array_vector();
	}
	std::vector<std::string> data_to_send = generate_outbound_class_data();
	for(uint_ c = 0;c < data_to_send.size();c++){ // optimize this later
		for(uint_ i = 0;i < client_vector_size;i++){
			client_t *tmp_client = (client_t*)find_pointer(client_vector[i], "client_t");
			if(tmp_client != nullptr){
				if(find_pointer(tmp_client->connection_info_id, "net_ip_connection_info_t") == nullptr){
					printf_("WARNING: Found an addressless client", PRINTF_UNLIKELY_WARN);
				}else{
					net->write(data_to_send[c], 0, tmp_client->connection_info_id);
				}
			}
		}
	}
}

static void net_read_data(){
	std::string data = "";
	while((data = net->read()) != ""){
		if(data.find_first_of(NET_JOIN) != std::string::npos){
			net_client_join(data);
		}else if(data.find_first_of(ARRAY_ITEM_SEPERATOR_START) != std::string::npos){
			update_class_data(data, ~0);
		}
	}
}

static void net_pingout(){
	std::vector<array_id_t> client_vector = all_ids_of_type("client_t");
	const uint_ client_vector_size = client_vector.size();
	for(uint_ i = 0;i < client_vector_size;i++){
		try{
			client_t *tmp_client = (client_t*)find_pointer(client_vector[i], "client_t");
			throw_if_nullptr(tmp_client);
			tmp_client->array.data_lock.lock();
			if(net_check_pingout(tmp_client)){
				delete tmp_client;
				tmp_client = nullptr;
			}
			tmp_client->array.data_lock.unlock();
		}catch(const std::logic_error &e){}
	}
}

int_ net_loop_settings = 0;

void net_engine(){
	loop_run(net_loop_code);
}

void net_close(){
	delete net;
	net = nullptr;
	delete net_loop_code;
	net_loop_code = nullptr;
	loop_del(server_loop_code, net_engine);
}
