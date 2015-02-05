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

static void net_check_and_apply_pingout(unsigned long int tmp_entry){
	client_t *tmp = (client_t*)(array_vector[tmp_entry]->pointer);
	const long double current_time = get_time();
	const long double pingout = 3; // timer is always in seconds
	printf("last_update: %Lf\n", tmp->array.last_update);
	printf("%Lf + 3\n", current_time);
	if(tmp->array.last_update == 0){
		return;
	}
	if(unlikely(tmp->array.last_update + pingout < current_time)){
		delete (coord_t*)find_pointer(tmp->coord_id);
		delete (model_t*)find_pointer(tmp->model_id);
		delete (net_ip_connection_info_t*)find_pointer(tmp->connection_info_id);
		delete tmp;
		tmp = nullptr;
		printf("client has pinged out\n");
	}
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
		last_client_net_run = get_time();
		client_t *client_tmp = new client_t;
		const long int old_id = client_tmp->connection_info_id;
		net_ip_connection_info_t *tmp_net_ip = new net_ip_connection_info_t;
		client_tmp->connection_info_id = tmp_net_ip->array.id;
		client_tmp->model_id = (new model_t)->array.id;
		client_tmp->coord_id = (new coord_t)->array.id;
		tmp_net_ip->array.parse_string_entry(data);
		tmp_net_ip->array.id = client_tmp->connection_info_id = old_id;
		printf("tmp_net_ip id is '%d'\n", tmp_net_ip->array.id);
		printf("tmp_net_ip.ip:%s\ttmp_net_ip.port: %d\n", tmp_net_ip->ip.c_str(), tmp_net_ip->port);
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
	int what_to_update = INT_MAX;
	SET_BIT(&what_to_update, ARRAY_INT_HASH_BIT, 1);
	SET_BIT(&what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 1);
	SET_BIT(&what_to_update, ARRAY_STRING_HASH_BIT, 1);
	for(unsigned long int c = 0;c < array_vector.size();c++){
		std::string gen_string = array_vector[c]->gen_updated_string(what_to_update);
		for(unsigned long int i = 0;i < array_vector.size();i++){
			if(unlikely(array_vector[i]->data_type == "client_t")){
				client_t *tmp_client = (client_t*)(array_vector[i]->pointer);
				net_ip_connection_info_t *tmp_net = (net_ip_connection_info_t*)find_pointer(tmp_client->connection_info_id);
				if(find_pointer(tmp_client->connection_info_id) == nullptr){
					printf("Could not find the client net_ip_connection_info class, not sending to said client\n");
				}else{
					net->write(gen_string, tmp_client->connection_info_id);
					printf("sent data (ID: %d) to a client (IP: %s)\n", array_vector[c]->id, tmp_net->ip.c_str());
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
