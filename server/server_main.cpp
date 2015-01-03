#include "server_main.h"
#include "server_physics.h"
#include "server_net.h"

net_ip_connection_info_t *self_info;
thread_t *thread;

bool terminate = false;

int argc_;
char **argv_;

static void write_vector_to_net(std::vector<std::vector<std::string>> *a){
	const unsigned long int a_size = a->size();
	for(unsigned long int i = 0;i < a_size;i++){
		const unsigned long int a_i_size = (*a)[i].size();
		for(unsigned long int n = 0;n < a_i_size;n++){
			const unsigned long int client_size = client_vector.size();
			for(unsigned long int c = 0;c < client_size;c++){
				net->write((*a)[i][n], client_vector[c]->connection_info_id, gen_rand());
			}
		}
	}
}

static void init_load_map_parse_line(std::string *a){
	if(a[0] == "coord"){
		new_coord();
		coord_t *b = coord_vector[coord_vector.size()-1];
		b->x = atof(a[1].c_str());
		b->y = atof(a[2].c_str());
		b->z = atof(a[3].c_str());
		b->set_x_angle(false,atof(a[4].c_str()));
		b->set_y_angle(false,atof(a[5].c_str()));
		b->array.id = atoi(a[6].c_str());
		b->model_id = atoi(a[7].c_str());
	}
}

static void init_load_map(std::string map_name = "test.map"){
	std::ifstream in(map_name);
	char data_[512];
	if(!in.is_open()){
		return;
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
	return;
}

static void init(){
	self_info = new_net_ip_connection_info();
	self_info->ip = "127.0.0.1";
	self_info->port = NET_SERVER_PORT;
	net = new net_t(argc_,argv_,self_info->array.id);
	thread = new thread_t;
	thread->init(argc_,argv_);
	net_init();
	init_load_map();
}

int scan_model_for_id(int id){
	for(unsigned int i = 0;i < model_vector.size();i++){
		if(model_vector[i]->array.id_match(id)){
			return i;
		}
	}
	return -1;
}

static void close(){
	net_close();
	delete net;
	net = nullptr;
	thread->close();
	delete thread;
	thread = nullptr;
}

static void class_engine_coord_update(){
	const unsigned long int coord_vector_size = coord_vector.size();
	for(unsigned long int i = 0;i < coord_vector_size;i++){
		std::vector<std::vector<std::string>> a = coord_vector[i]->array.gen_string_vector();
		write_vector_to_net(&a);
	}

}

static void class_engine_model_update(){
	const unsigned long int model_vector_size = model_vector.size();
	for(unsigned long int i = 0;i < model_vector_size;i++){
		std::vector<std::vector<std::string>> a = model_vector[i]->array.gen_string_vector();
		write_vector_to_net(&a);
	}
}

static void class_engine_client_update(){
	const unsigned long int client_vector_size = client_vector.size();
	for(unsigned long int i = 0;i < client_vector_size;i++){
		std::vector<std::vector<std::string>> a = client_vector[i]->array.gen_string_vector();
		write_vector_to_net(&a);
	}
}

static void class_engine(){
	class_engine_client_update();
	class_engine_model_update();
	class_engine_coord_update();
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	#ifdef __linux
	//signal(SIGINT, signal_handler);
	#endif
	init();
	printf("Starting the main loop\n");
	while(terminate == false){
		physics_engine();
		net_engine();
		class_engine();
	}
	close();
	return 0;
}
