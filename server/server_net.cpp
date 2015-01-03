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

static void net_check_and_apply_pingout(unsigned long int tmp_entry, unsigned long int *client_size){
	client_t *tmp = client_vector[tmp_entry];
	const long double current_time = get_time();
	const long double pingout = 3;
	if(tmp->array.last_update > current_time + pingout){

		delete_client_id(tmp->array.id);
		(*client_size) -= 1;
	}
}

static void net_client_join(std::string data){
	printf("Data:'%s'\n",data.c_str());
	client_t *client_tmp = new_client();
	find_net_ip_connection_info_pointer(client_tmp->connection_info_id)->array.parse_string_entry(data);
	std::string return_packet = NET_JOIN + std::to_string(client_tmp->array.id);
	net->write(return_packet, client_tmp->connection_info_id, gen_rand());
}

void net_init(){
	if(net_init_bool == false){
		net_init_bool = true;
		class_data_settings |= 0 << 0;
		class_data_settings |= 0 << 1;
	}else{
		printf("net_init has already been initalized, not re-running the initializer\n");
	}
}

void net_engine(){
	if(unlikely(net_init_bool == false)){
		printf("The engine has not initialized before the first engine instance, initializing it now. Check to see if the close statement is still there\n");
		net_init();
	}
	net->loop();
	std::string data = "";
	while((data = net->read()) != ""){
		if(data.find_first_of(NET_JOIN) != std::string::npos){
			net_client_join(data);
		}else if(data.find_first_of(ARRAY_ITEM_SEPERATOR_START) != std::string::npos){
			update_class_data(data, class_data_settings);
		}else{
			printf("Could not identify what this packet is, scraping it and moving forward\n");
		}
	}
	unsigned long int client_size = client_vector.size();
	for(unsigned long int i = 0;i < client_size;i++){
		net_check_and_apply_pingout(i, &client_size);
	}
	unsigned long int array_size = array_vector.size();
	for(unsigned long int c = 0;c < array_size;c++){
		int what_to_update = INT_MIN;
		if(array_vector[c]->updated(&what_to_update)){
			std::string gen_string;
			if(CHECK_BIT(ARRAY_INT_HASH_BIT, 1)){
				printf("Int data was updated\n");
				gen_string += array_vector[c]->gen_int_string();
			}
			if(CHECK_BIT(ARRAY_LONG_DOUBLE_HASH_BIT, 1)){
				printf("Long double was updated\n");
				gen_string += array_vector[c]->gen_long_double_string();
			}
			if(CHECK_BIT(ARRAY_STRING_HASH_BIT, 1)){
				printf("String was updated\n");
				gen_string += array_vector[c]->gen_string_string();
			}
			for(unsigned long int i = 0;i < client_size;i++){
				net_ip_connection_info_t *tmp_net = find_net_ip_connection_info_pointer(client_vector[i]->connection_info_id);
				if(tmp_net == nullptr){
					printf("Could not find the client net_ip_connection_info class, not sending to said client\n");
				}else{
					net->write(gen_string, client_vector[i]->connection_info_id, gen_rand());
				}
			}
		}
	}
}

void net_close(){}
