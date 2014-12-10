#include "server_main.h"
#include "server_physics.h"
#include "server_net.h"
#include "server_console.h"
#include "signal.h"

std::vector<coord_t*> coord;
std::vector<coord_extra_t> coord_extra;
std::vector<model_t*> model;
std::vector<model_extra_t> model_extra;
std::vector<client_t*> client;
std::vector<client_extra_t> client_extra;

thread_t *thread;

bool terminate = false;

int argc_;
char **argv_;

static void init_load_map_parse_line(std::string *a){
	if(a[0] == "coord"){
		new_init_coord_t();
		coord_t *b = coord[coord.size()-1];
		b->x = atof(a[1].c_str());
		b->y = atof(a[2].c_str());
		b->z = atof(a[3].c_str());
		b->set_x_angle(false,atof(a[4].c_str()));
		b->set_y_angle(false,atof(a[5].c_str()));
		b->array->id = atoi(a[6].c_str());
		b->model_id = atoi(a[7].c_str());
	}
}

static void init_load_map(std::string map_name = "test.map"){
	std::ifstream in(map_name);
	char data_[512];
	if(!in.is_open()){
		goto init_load_map_end;
	}
	while(in.getline(data_,511)){
		data_[511] = '\0';
		std::string data = data_;
		std::string parse[8] = {""};
		std::stringstream ss;
		ss << data;
		ss >> parse[0] >> parse[1] >> parse[2] >> parse[3] >> parse[4] >> parse[5] >> parse[6] >> parse[7];
		init_load_map_parse_line(parse);
	}
	init_load_map_end:;
}

static void init_load_model_parse_line(std::string *a){
	if(a[0] == "model"){
		new_init_model_t();
		model_t *b = model[model.size()-1];
		b->load(a[1]);
		unsigned long int c = atoi(a[2].c_str());
		b->array->id = c;
	}
}

static void init_load_models(std::string model_list_name = "model.list"){
	std::ifstream in(model_list_name);
	if(!in.is_open()){
		goto init_load_models_end;
	}
	char data_[512];
	while(in.getline(data_,511)){
		data_[511] = '\0';
		std::string data = data_;
		std::string parse[8];
		std::stringstream ss;
		ss << data;
		ss >> parse[0] >> parse[1] >> parse[2] >> parse[3] >> parse[4] >> parse[5] >> parse[6] >> parse[7];
		init_load_model_parse_line(parse);
	}
	init_load_models_end:;
}

static void init(){
	net = new net_t(argc_,argv_);
	if(net->ip->connection_info.port == 0){
		net->ip->connection_info.port = NET_IP_SERVER_RECEIVE_PORT;
	}
	if(net->ip->connection_info.ip == ""){ // local or global?
		net->ip->connection_info.ip = "127.0.0.1";
	}
	if(net->ip->connection_info.connection_type == NET_IP_CONNECTION_TYPE_UNDEFINED){
		net->ip->connection_info.connection_type = NET_IP_CONNECTION_TYPE_UDP;
	}
	console_init();
	thread = new thread_t;
	thread->init(argc_,argv_);
	init_load_models();
	init_load_map();
}

int scan_model_for_id(int id){
	for(unsigned int i = 0;i < model.size();i++){
		if(model[i]->array->id_match(id)){
			return i;
		}
	}
	return -1;
}

static void close(){
	net->close();
	delete net;
	net = nullptr;
	const int coord_size = coord.size();
	for(int i = 0;i < coord_size;i++){
		coord[i]->close();
		delete coord[i];
		coord[i] = nullptr;
	}
	const int model_size = model.size();
	for(int i = 0;i < model_size;i++){
		model[i]->close();
		delete model[i];
		model[i] = nullptr;
	}
	const int client_size = client.size();
	for(int i = 0;i < client_size;i++){
		client[i]->close();
		delete client[i];
		client[i] = nullptr;
	}
	console_close();
	thread->close();
	delete thread;
	thread = nullptr;
}

void signal_handler(int signal){
	if(signal == SIGINT){
		printf("Received SIGINT\n");
		terminate = true;
	}
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	init();
	signal(SIGINT, signal_handler);
	for(unsigned long int i = 0;i < 1024;i++){
		new_init_coord_t();
		unsigned long int a = coord.size()-1;
		coord[a]->x = gen_rand();
		coord[a]->y = gen_rand();
		coord[a]->z = gen_rand();
	}
	printf("Starting the main loop\n");
	while(terminate == false){
		physics_engine();
		net_engine();
		console_engine();
	}
	close();
	ms_sleep(1000); // wait for threads without references to stop
	return 0;
}
