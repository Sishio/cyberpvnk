#include "server_main.h"
#include "server_console.h"
#include "server_physics.h"
#include "server_net.h"

int loop_settings;

server_info_t *server_info = nullptr;
net_ip_connection_info_t *self_info = nullptr; // TODO: rename this to prevent confusion with server_info

loop_t server_loop_code;

int argc_;
char **argv_;

coord_t *map = nullptr;

server_info_t::server_info_t(){}

server_info_t::~server_info_t(){}

/*static void write_vector_to_net(std::vector<std::vector<std::string>> *a){
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
}*/

void server_info_init(){
	server_info = new server_info_t; // nothing special here until I decide to send this over to the client
	for(int i = 0;i < argc_;i++){
		char *next_item;
		if(i+i == argc_){
			next_item = (char*)"";
		}else{
			next_item = argv_[i+1];
		}
		if(strcmp(argv_[i],"--server-info-map-path") == 0){
			server_info->map_path = next_item;
		}else if(strcmp(argv_[i], "--server-info-map-name") == 0){
			server_info->map_name = next_item;
		}
	}
}

void test_logic_engine();

void test_logic_init(){
	loop_add(&server_loop_code, "test_logic_engine", test_logic_engine);
	net_ip_connection_info_t *tmp_conn_info = new net_ip_connection_info_t;
	tmp_conn_info->ip = "127.0.0.1";
	tmp_conn_info->port = NET_IP_SERVER_RECEIVE_PORT;
	net = new net_t(argc_, argv_, tmp_conn_info->array.id);
}

void test_logic_engine(){
	coord_t tmp_coord[2];
	tmp_coord[0].array.data_lock.lock();
	tmp_coord[0].array.long_double_lock.lock();
	for(unsigned long int i = 1;i < tmp_coord[0].array.long_double_array.size();i++){
		*tmp_coord[0].array.long_double_array[i] = (long double)gen_rand();
	}
	tmp_coord[0].array.long_double_lock.unlock();
	tmp_coord[0].array.data_lock.unlock();
	net_ip_connection_info_t tmp_net_conn_info;
	tmp_net_conn_info.ip = "127.0.0.1";
	tmp_net_conn_info.port = NET_IP_SERVER_RECEIVE_PORT;
	net->write(tmp_coord[0].array.gen_updated_string(UINT_MAX), tmp_net_conn_info.array.id);
	tmp_coord[1].array.id = tmp_coord[0].array.id;
	tmp_coord[0].array.id++;
	std::string incoming_packet;
	while((incoming_packet = net->read()) == ""){
		net->loop();
	}
	update_class_data(incoming_packet, CLASS_DATA_UPDATE_EVERYTHING);
	tmp_coord[0].print();
	tmp_coord[1].print();
}

void test_logic_close(){
	loop_del(&server_loop_code, test_logic_engine);
	delete net;
	net = nullptr;
}

void simple_signal_handler(int signum){
	terminate = true;
}

static void load_previous_server_state(){
	std::ifstream in("server_state.save");
	if(in.is_open()){
		std::vector<std::string> save;
		char data[65536];
		while(in.getline(data, 65536)){
			save.push_back(data);
		}
		const unsigned long int save_size = save.size();
		for(unsigned long int i = 0;i < save_size;i++){
			update_class_data(save[i], UINT_MAX);
			update_progress_bar(i/save_size);
		}
	}else{
		printf("There doesn't appear to be a server_save file to use\n");
	}
}

static void load_initial_values(){
	std::ifstream in("server_settings.save");
	if(in.is_open()){
		char data[512];
		while(in.getline(data, 512)){
			std::stringstream ss(data);
			std::string data_[2];
			ss >> data_[0] >> data_[1];
			long long int data_num = atoi(data_[1].c_str());
			if(data_[0] == "loop_settings"){
				loop_settings = data_num;
			}else if(data_[0] == "net_loop_settings"){
				net_loop_settings = data_num;
			}
		}
		in.close();
	}
}

void init(int choice){
	load_previous_server_state();
	server_loop_code.name = "server loop code";
	//signal(SIGINT, simple_signal_handler);
	load_initial_values();
	switch(choice){
	case 1:
		server_info_init();
		net_init();
		physics_init("gametype/default");
		console_init();
		break;
	case 2:
		test_logic_init();
		break;
	case 3:
		terminate = true;
		break;
	default:
		printf("WARNING: This was NOT one of the options, terminating\n");
		terminate = true;
		break;
	}
}

void close(){
	net_close();
	physics_close();
	console_close();
	test_logic_close();
	delete_all_data();
	// the functions are smart enough to not close if they never initialized
}

int menu(){
	int return_value;
	printf("(1) Start the server normally\n");
	printf("(2) Start the test_logic\n");
	printf("(3) Exit\n");
	scanf("%d", &return_value);
	return return_value;
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	init(menu());
	printf("Starting the main loop\n");
	SET_BIT(loop_settings, LOOP_CODE_MT, 0);
	while(terminate == false){
		loop_run(&server_loop_code, &loop_settings);
		once_per_second_update();
	}
	close();
	return 0;
}
