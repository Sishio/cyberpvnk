#include "server_net.h"

net_t *net;

static void store_coord_t(coord_t a){
	const int coord_size = coord.size();
	for(int i = 0;i < coord_size;i++){
		if(coord[i] != nullptr){
			if(a.array->id_match(coord[i]->array->id)){
				memcpy(&coord[i],&a,sizeof(coord_t));
				goto store_coord_t_end;
			}
		}
	}
	for(int i = 0;i < coord_size;i++){
		if(coord[i] == nullptr){
			coord[i] = new coord_t;
			memcpy(&coord[i],&a,sizeof(coord_t));
			goto store_coord_t_end;
		}
	}
	coord.push_back(new coord_t);
	memcpy(&coord[coord_size+1],&a,sizeof(coord_t));
	store_coord_t_end:;
}

static void store_model_t(model_t a){
	const int model_size = model.size();
	for(int i = 0;i < model_size;i++){
		if(model[i] != nullptr){
			if(a.array->id_match(model[i]->array->id)){
				memcpy(&model[i],&a,sizeof(model_t));
				goto store_model_t_end;
			}
		}
	}
	for(int i = 0;i < model_size;i++){
		if(model[i] == nullptr){
			model[i] = new model_t;
			memcpy(&model[i],&a,sizeof(model_t));
			goto store_model_t_end;
		}
	}
	model.push_back(new model_t);
	memcpy(&model[model_size+1],&a,sizeof(model_t));
	store_model_t_end:;
}

// sends everyone every coord on the server
// this should be used sparringly
// I need to have a flag that is set on every modified pointer and send those to everybody
// or I could just tell the client to send it to another client and use their bandwidth

static void send_coord_t(){
	const int coord_size = coord.size();
	for(int i = 0;i < coord_size;i++){
		if(coord[i] != nullptr && coord_extra[i].updated){
			std::string data = coord[i]->array->gen_string();
			const int client_size = client.size();
			for(int n = 0;n < client_size;n++){
				if(client[n] != nullptr){
					net->write(data,client[n]->net.port,client[n]->net.ip);
				}
			}
		}
	}
}

static void send_model_t(){
	const int model_size = model.size();
	for(int i = 0;i < model_size;i++){
		if(model[i] != nullptr && model_extra[i].updated){
			std::string data = model[i]->array->gen_string();
			const int client_size = client.size();
			for(int n = 0;n < client_size;n++){
				if(client[i] != nullptr){
					net->write(data,client[i]->net.port,client[n]->net.ip);
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
				a.init();
				a.array->parse_string(data);
				store_coord_t(a);
			}
			break;
		case ARRAY_HEADER_MODEL_T:
			if(true){
				model_t a;
				a.init();
				a.array->parse_string(data);
				store_model_t(a);
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
