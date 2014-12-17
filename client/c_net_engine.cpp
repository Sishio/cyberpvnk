#include "c_engine.h"
#include "c_net_engine.h"

static net_ip_connection_info_t server_connection_info;
static coord_t *old_coord = nullptr;

static void net_engine_parse(std::string a){
	update_data(a);
}

static void net_write_array_vector(std::vector<std::vector<std::string>> a, net_ip_connection_info_t b){
	const unsigned long int a_size = a.size();
	for(unsigned long int i = 0;i < a_size;i++){
		const unsigned long int a_i_size = a[i].size();
		for(unsigned long int n = 0;n < a_i_size;n++){
			net->write(a[i][n],b);
		}
	}
}

void net_init(){
	for(int i = 0;i < argc_;i++){
		if(strcmp(argv_[i], (char*)"--net-ip-server-ip") == 0){
			server_connection_info.ip = (std::string)argv_[i+1];
		}else if(strcmp(argv_[i], (char*)"--net-ip-server-port") == 0){
			server_connection_info.port = atoi((char *)argv_[i+1]);
		}
	}
}

void net_engine(){
	server_connection_info.ip = "127.0.0.1";
	server_connection_info.port = NET_IP_SERVER_RECEIVE_PORT;
	std::string a;
	while((a = net->read()) != ""){
		net_engine_parse(a);
	}
	if(unlikely(old_coord == nullptr)){
		old_coord = new coord_t;
	}
	coord_t *coord = find_coord_pointer(self->coord_id);
	if(coord != nullptr){
		if(likely(memcmp(&old_coord,coord,sizeof(coord_t)) != 0)){
			std::vector<std::vector<std::string>> a = self->array->gen_string_vector();
			net_write_array_vector(a, server_connection_info);
		}
		memcpy(&old_coord,coord,sizeof(coord_t)); //padding
	}
}

void net_close(){
}
