#include "server_net.h"

net_t *net;

static void send_coord_t(){
	const unsigned long int coord_size = coord.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		if(coord[i] != nullptr && coord_extra[i].updated){
			std::string data = coord[i]->array->gen_string();
			const unsigned long int client_size = client.size();
			for(unsigned long int n = 0;n < client_size;n++){
				if(client[n] != nullptr){
					net->write(data,client[n]->net_ip_connection_info);
				}
			}
		}
	}
}

static void send_model_t(){
	const unsigned long int model_size = model.size();
	for(unsigned long int i = 0;i < model_size;i++){
		if(model[i] != nullptr && model_extra[i].updated){
			std::string data = model[i]->array->gen_string();
			const unsigned long int client_size = client.size();
			for(unsigned long int n = 0;n < client_size;n++){
				if(client[i] != nullptr){
					net->write(data,client[n]->net_ip_connection_info);
				}
			}
		}
	}
}

void net_engine(){
	// remember, the net->loop is in another thread that started with net->init
	std::string data = "";
	while((data = net->read()) != ""){
		switch(data[0]){
		case ARRAY_HEADER_COORD_T:
			if(true){
				coord_t a;
				a.array->parse_string(data);
				store_coord_t(a,&coord,&coord_extra);
			}
			break;
		case ARRAY_HEADER_MODEL_T:
			if(true){
				model_t a;
				a.array->parse_string(data);
				store_model_t(a,&model,&model_extra);
			}
			break;
		//case ARRAY_HEADER_CLIENT_T:
		//	if(true){
		//		client_t a;
		//		a.init();
		//		a.array->parse_string(data);
		//		store_client_t(a);
		//	}
		//	break;
		default: // either no data or the first digit is zero
			break;
		}
	}
	send_model_t();
	send_coord_t(); // everything
}
