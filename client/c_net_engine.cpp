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

net_ip_connection_info_t server_connection_info;
//static coord_t *old_coord = nullptr;
extern bool once_per_second;

/*static void net_engine_parse(std::string a){
	if(a.find_first_of(ARRAY_ITEM_SEPERATOR_START) == std::string::npos){
		update_class_data(a); // universal fucntion for everything data related
	}else{
		printf("Unknown data was received\n");
	}
}

static void net_write_array_vector(std::vector<std::vector<std::string>> a, net_ip_connection_info_t b){
	const unsigned long int a_size = a.size();
	for(unsigned long int i = 0;i < a_size;i++){
		const unsigned long int a_i_size = a[i].size();
		for(unsigned long int n = 0;n < a_i_size;n++){
			net->write(a[i][n],b);
		}
	}
}*/

void net_init(){
	server_connection_info.ip = "127.0.0.1";
	server_connection_info.port = 50000;
	for(int i = 0;i < argc_;i++){
		if(strcmp(argv_[i], (char*)"--net-ip-server-ip") == 0){
			server_connection_info.ip = (std::string)argv_[i+1];
		}else if(strcmp(argv_[i], (char*)"--net-ip-server-port") == 0){
			server_connection_info.port = atoi((char *)argv_[i+1]);
		}
	}
}

void net_engine(){
  /*	std::string a;
	while((a = net->read()) != ""){
		net_engine_parse(a);
	}
	if(unlikely(old_coord == nullptr)){
		old_coord = new coord_t; // don't add this to the main array
	}
	coord_t *coord = find_coord_pointer(self->coord_id);
	if(coord != nullptr){
		if(likely(memcmp(&old_coord,coord,sizeof(coord_t)) != 0)){
			std::vector<std::vector<std::string>> a = self->array->gen_string_vector(once_per_second);
			// true means it forces itself to rewrite everything, false means only rewrite it if the data has changed
			net_write_array_vector(a, server_connection_info);
		}
		memcpy(&old_coord,coord,sizeof(coord_t)); //includes padding
	}*/
}

void net_close(){
}
