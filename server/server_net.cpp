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
static int class_data_settings = CLASS_DATA_UPDATE_EVERYTHING;
net_t *net = nullptr;
static loop_t net_loop_code;
extern loop_t server_loop_code;
extern std::vector<array_t*> array_vector;

static inline void net_check_and_apply_pingout(unsigned long int tmp_entry){
	client_t *tmp = (client_t*)(array_vector[tmp_entry]->pointer);
	const long double current_time = get_time();
	const long double pingout = 3;
	if(unlikely(tmp->array.last_update > current_time + pingout)){
		delete tmp;
	}
}

static void net_client_join(std::string data){
	client_t *client_tmp = new client_t;
	net_ip_connection_info_t *connection_pointer = (net_ip_connection_info_t*)find_array_pointer(client_tmp->connection_info_id);
	printf("data:%s\n",data.c_str());
	connection_pointer->array.parse_string_entry(data);
	client_tmp->connection_info_id = connection_pointer->array.id;
	client_tmp->model_id = (new model_t)->array.id;
	client_tmp->coord_id = (new coord_t)->array.id;
	std::string return_packet = NET_JOIN + std::to_string(client_tmp->array.id);
	printf("New client IP address: %s\n", connection_pointer->ip.c_str());
	printf("New client port address: %d\n", connection_pointer->port);
	net->write(return_packet, client_tmp->connection_info_id);
}

static void net_send_data();
static void net_pingout();
static void net_read_data();

void net_init(){
	if(net_init_bool == false){
		net_ip_connection_info_t *tmp_conn_info = new net_ip_connection_info_t;
		tmp_conn_info->ip = "127.0.0.1";
		tmp_conn_info->port = NET_SERVER_PORT;
		net = new net_t(argc_, argv_, tmp_conn_info->array.id);
		net_init_bool = true;
		SET_BIT(&class_data_settings, CLASS_DATA_COORD_BIT, 1);
		SET_BIT(&class_data_settings, CLASS_DATA_MODEL_BIT, 1);
		SET_BIT(&class_data_settings, CLASS_DATA_CLIENT_BIT, 1);
		loop_code.push_back(net_engine);
		loop_add(&net_loop_code, net_read_data);
		loop_add(&net_loop_code, net_pingout);
		loop_add(&net_loop_code, net_send_data);
		loop_add(&server_loop_code, net_engine);
	}else{
		printf("net_init has already been initalized, not re-running the initializer\n");
	}
}

static void net_send_data(){
	int blank_what_to_update;
	SET_BIT(&blank_what_to_update, ARRAY_INT_HASH_BIT, 0);
	SET_BIT(&blank_what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 0);
	SET_BIT(&blank_what_to_update, ARRAY_STRING_HASH_BIT, 0);
	int what_to_update = INT_MAX;
	for(unsigned long int c = 0;c < array_vector.size();c++){
		if(array_vector[c]->updated(&what_to_update)){
			std::string gen_string;
			if(CHECK_BIT(what_to_update, ARRAY_INT_HASH_BIT, 1)){
				gen_string += array_vector[c]->gen_int_string();
			}
			if(CHECK_BIT(what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 1)){
				gen_string += array_vector[c]->gen_long_double_string();
			}
			if(CHECK_BIT(what_to_update, ARRAY_STRING_HASH_BIT, 1)){
				gen_string += array_vector[c]->gen_string_string();
			}
			for(unsigned long int i = 0;i < array_vector.size();i++){
				if(unlikely(array_vector[i]->data_type == "client_t")){
					net_ip_connection_info_t *tmp_net = (net_ip_connection_info_t*)find_array_pointer(((client_t*)array_vector[i]->pointer)->connection_info_id);
					if(tmp_net == nullptr){
						printf("Could not find the client net_ip_connection_info class, not sending to said client\n");
					}else{
						net->write(gen_string, ((client_t*)array_vector[i]->pointer)->connection_info_id);
					}
				}
			}
		}
	}

}

static void net_read_data(){
	std::string data = "";
	while((data = net->read()) != ""){
		// switch over to a string based system
		if(data.find_first_of(NET_JOIN) != std::string::npos){
			net_client_join(data);
		}else if(data.find_first_of(ARRAY_ITEM_SEPERATOR_START) != std::string::npos){
			update_class_data(data, class_data_settings);
		}else{
			printf("Could not identify what this packet is, scraping it and moving forward\n");
		}
	}
}

static void net_pingout(){
	for(unsigned long int i = 0;i < array_vector.size();i++){
		if(unlikely(array_vector[i]->data_type == "client_t")){
			net_check_and_apply_pingout(i);
		}
	}
}

void net_engine(){
	net->loop();
	int settings = 1;
	loop_run(&net_loop_code, settings);
}

void net_close(){
	for(unsigned long int i = 0;i < loop_code.size();i++){
		if(loop_code[i] == net_engine){
			loop_code[i] = nullptr;
			loop_code.erase(loop_code.begin()+i);
			break;
		}
	}
}
