#include "server_main.h"
#include "server_console.h"
#include "server_input.h"
#include "server_physics.h"
#include "server_net.h"
#include "server_gametype.h"

int_ loop_settings;

server_info_t *server_info = nullptr;
net_ip_connection_info_t *self_info = nullptr; // TODO: rename this to prevent confusion with server_info

loop_t server_loop_code;

int_ argc_;
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
	for(int_ i = 0;i < argc_;i++){
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
	coord_t tmp[1024];
	for(int_ i = 0;i < 1024;i++){
		find_pointer(find_array_pointer(tmp[i].array.id)->id);
	}
}

void test_logic_close(){
	loop_del(&server_loop_code, test_logic_engine);
}

static void load_previous_server_state(){
	std::ifstream in("server_state.save");
	if(in.is_open()){
		std::vector<std::string> save;
		char data[65536];
		while(in.getline(data, 65536)){
			save.push_back(data);
		}
		const uint_ save_size = save.size();
		for(uint_ i = 0;i < save_size;i++){
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
			int_ data_num = atoi(data_[1].c_str());
			if(data_[0] == "loop_settings"){
				loop_settings = data_num;
			}else if(data_[0] == "net_loop_settings"){
				net_loop_settings = data_num;
			}
		}
		in.close();
	}
}

void reserve_ids(){
	array_t *iterator = new array_t(nullptr, false);
	iterator->data_type = "server_iterator";
	iterator->int_array.push_back(&server_loop_code.tick);
	iterator->new_id(RESERVE_ID_ITERATOR);
	iterator->immunity(true);
	// the rest aren't reserved, but should follow the same rules
	array_t *loop_settings_ = new array_t(nullptr, true);
	loop_settings_->int_array.push_back(&loop_settings);
	loop_settings_->data_type = "loop_settings";
	loop_settings_->immunity(true);
}

void init(int_ choice){
	reserve_ids();
	console_init();
	load_previous_server_state();
	server_loop_code.name = "server loop code";
	//signal(SIGINT, simple_signal_handler);
	load_initial_values();
	switch(choice){
	case 1:
		server_info_init();
		if(check_for_parameter("--net-disable", argc_, argv_) == false) net_init();
		if(check_for_parameter("--physics-disable", argc_, argv_) == false) physics_init();
		if(check_for_parameter("--gametype-disable", argc_, argv_) == false) gametype_init();
		if(check_for_parameter("--input-disable", argc_, argv_) == false) input_init(); // signals should still work from the console
		break;
	case 2:
		test_logic_init();
		break;
	case 3:
		set_signal(SIGTERM, true);
		break;
	default:
		printf("WARNING: This was NOT one of the options, terminating\n");
		set_signal(SIGTERM, true);
		break;
	}
}

void close(){
	net_close();
	input_close();
	physics_close();
	console_close();
	test_logic_close();
	delete_all_data();
	// the functions are smart enough to not close if they never initialized
}

int_ menu(){
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
	SET_BIT(loop_settings, LOOP_CODE_PARTIAL_MT, 0);
	while(likely(check_signal(SIGINT) == false && check_signal(SIGKILL) == false && check_signal(SIGTERM) == false)){
		loop_run(&server_loop_code, &loop_settings);
		once_per_second_update();
	}
	close();
	return 0;
}
