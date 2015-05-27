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

#include "server_net.h"

static bool net_init_bool = false;
static int_ class_data_settings = CLASS_DATA_UPDATE_EVERYTHING;
net_t *net = nullptr;
static loop_t net_loop_code;
extern loop_t server_loop_code;

static bool net_check_pingout(client_t *tmp){
	const long double current_time = get_time();
	const long double pingout = 3; // timer is always in seconds
	return tmp->array.last_update + pingout < current_time;
}

static double last_client_net_run = 0;
#define CLIENT_MAX_JOIN_RATE .2

static void net_client_join(std::string data){
	bool accept_client = false;
	if(last_client_net_run != 0){
		double time_test = get_time()-last_client_net_run;
		accept_client = time_test*CLIENT_MAX_JOIN_RATE >= 1;
	}else{
		accept_client = true;
	}
	if(likely(accept_client)){
		printf("Accepted the client\n");
		last_client_net_run = get_time();
		client_t *client_tmp = new client_t;
		net_ip_connection_info_t *tmp_net_ip = new net_ip_connection_info_t;
		tmp_net_ip->array.parse_string_entry(data);
		client_tmp->connection_info_id = tmp_net_ip->array.id;
		client_tmp->model_id = (new model_t)->array.id;
		client_tmp->coord_id = (new coord_t)->array.id;
		std::cout << "tmp_net_ip id is " << tmp_net_ip->array.id << std::endl;
		//printf("tmp_net_ip.ip:%s\ttmp_net_ip.port: %d\n", tmp_net_ip->ip.c_str(), tmp_net_ip->port);
		std::string return_packet = NET_JOIN + std::to_string(client_tmp->array.id);
		net->write(return_packet, client_tmp->connection_info_id);
	}else{
		printf("someone is spamming the server with join requests\n");
	}
}

static void net_send_data();
static void net_pingout();
static void net_read_data();

void net_init(){
	if(net_init_bool == false){
		net_loop_code.name = "net loop code";
		net_ip_connection_info_t *tmp_conn_info = new net_ip_connection_info_t;
		tmp_conn_info->ip = "127.0.0.1";
		tmp_conn_info->port = NET_SERVER_PORT;
		net = new net_t(argc_, argv_, tmp_conn_info->array.id);
		net_init_bool = true;
		SET_BIT(class_data_settings, CLASS_DATA_COORD_BIT, 1);
		SET_BIT(class_data_settings, CLASS_DATA_MODEL_BIT, 1);
		SET_BIT(class_data_settings, CLASS_DATA_CLIENT_BIT, 1);
		loop_add(&net_loop_code, "net_read_data", net_read_data);
		loop_add(&net_loop_code, "net_pingout", net_pingout);
		loop_add(&net_loop_code, "net_send_data",  net_send_data);
		loop_add(&server_loop_code, "net_engine",  net_engine);
	}else{
		printf("net_init has already been initalized, not re-running the initializer\n");
	}
}

static void net_send_data(){
	int_ what_to_update = INT_MAX;
	std::vector<array_id_t> client_vector = all_ids_of_type("client_t");
	const uint_ client_vector_size = client_vector.size();
	if(client_vector_size == 0){
		return;
	}
	if(once_per_second){
		const std::string reset_vector = wrap(ARRAY_FUNCTION_START, "reset_vector", ARRAY_FUNCTION_END);
		for(uint_ i = 0;i < client_vector_size;i++){
			net->write(reset_vector, ((client_t*)find_pointer(client_vector[i], "coord_t"))->connection_info_id);
		}
	}
	std::vector<std::string> data_to_send = generate_outbound_class_data();
	for(uint_ c = 0;c < data_to_send.size();c++){
		for(uint_ i = 0;i < client_vector_size;i++){
			client_t *tmp_client = (client_t*)find_pointer(client_vector[i], "client_t");
			if(tmp_client != nullptr){
				if(find_pointer(tmp_client->connection_info_id, "net_ip_connection_info_t") == nullptr){
					printf_("WARNING: Found an addressless client", PRINTF_UNUSUAL_WARN);
				}else{
					net->write(gen_string, tmp_client->connection_info_id);
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
			update_class_data(data, class_data_settings);
		}
	}
}

static void net_pingout(){
	std::vector<array_id_t> client_vector = all_ids_of_type("client_t");
	const uint_ client_vector_size = client_vector.size();
	for(uint_ i = 0;i < client_vector_size;i++){
		((client_t*)find_pointer(client_vector[i], "client_t"))->array.data_lock.lock();
		if(net_check_pingout((client_t*)find_pointer(client_vector[i], "client_t")) == true){
				((client_t*)find_pointer(client_vector[i], "client_t"))->array.data_lock.unlock();
				delete (client_t*)find_pointer(client_vector[i], "client_t");
		}else{
				((client_t*)find_pointer(client_vector[i], "client_t"))->array.data_lock.unlock();
		}
	}
}

int_ net_loop_settings = 0;

void net_engine(){
	net->loop();
	//SET_BIT(net_loop_settings, LOOP_CODE_MT, 1);
	loop_run(&net_loop_code, &net_loop_settings);
}

void net_close(){
	delete net;
	net = nullptr;
	loop_del(&server_loop_code, net_engine);
}
