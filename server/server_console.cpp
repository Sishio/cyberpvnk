#include "../net/net_main.h"
extern net_t *net;
#include "server_console.h"

static std::thread console_thread;
static std::string line;
static std::string command[8];
static std::string in_stack[8];
static std::string out_stack[8];
static int_* pointer_stack_int[8] = {nullptr};
static long double* pointer_stack_long_double[8] = {nullptr};
static std::string* pointer_stack_string[8] = {nullptr};
static void *last_used_pointer = nullptr;

#define STD_INVALID_ARGUMENT_ERROR	"An error was reported: std::invalid_argument\n"
#define OUT_OF_BOUNDS_ERROR			"An error was reported: you have requested an out-of-bounds item\n"
#define INVALID_OBJECT_ERROR		"An error was reported: the object you requested doesn't exist\n"
#define NOT_IMPLEMENTED_ERROR		"An error was reported: the command you have given has not been implemented yet\n"

void console_init(){
	console_thread = std::thread(console_engine);
}

int run_command();

void console_engine(){
	while(true){
		std::cout << "]";
		getline(std::cin, command[0]);
		std::stringstream ss(command[0]);
		ss >> command[0] >> command[1] >> command[2] >> command[3] >> command[4] >> command[5] >> command[6] >> command[7];
		long double start_time = get_time();
		if(run_command() == 0){
			std::cout << "run_command finished in " << get_time()-start_time << "s" << std::endl;
		}else{
			std::cout << "run_command failed" << std::endl;
		}
		for(unsigned long int i = 0;i < 8;i++){
			command[i] = "";
		}
	}
}

void console_close(){
}

std::string string_to_decimal_string(std::string a){
	std::string return_value;
	if(a.find_first_of("x") == 1){
		int_ num = std::strtoll(a.c_str(), nullptr, 16); // do I chop off the 0x?
		return_value = std::to_string(num);
	}else if(a.find_first_of("b") == 1){ // prefixed wiht 0b
		int_ num = std::strtoll(a.substr(2, a.size()).c_str(), nullptr, 2);
		return_value = std::to_string(num);
	}else{
		return_value = a;
	}
	return return_value;
}

std::string get_target_value(std::string target_value, void* thing, std::string type){
	const bool psi_ = target_value.find_first_of("_PSI") != std::string::npos;
	const bool pss_ = target_value.find_first_of("_PSS") != std::string::npos;
	const bool psld_ = target_value.find_first_of("_PSLD") != std::string::npos;
	const bool os_ = target_value.find_first_of("_OS") != std::string::npos;
	const bool is_ = target_value.find_first_of("_IS") != std::string::npos;
	const bool nil_ = target_value == "NIL";
	last_used_pointer = thing;
	if(!psi_ && !pss_ && !psld_ && !os_ && !is_ && !nil_){
		return target_value; // no special conditions have been met
	}
	std::string return_value;
	if(type == "string"){
		return_value = *((std::string*)thing);
	}else if(type == "long_double"){
		return_value = std::to_string(*((long double*)thing));
	}else if(type == "int"){
		return_value = std::to_string(*((int_*)thing)); //you can convert down, I think
	}
	if(nil_){
		return return_value; // worst case scenario: nothing changes
	}
	int_ position_in_stack;
	try{
		position_in_stack = std::stoll(target_value);
	}catch(std::invalid_argument){
		printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
		return return_value;
	}
	if(position_in_stack > 7 && position_in_stack < 0){
		printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
		return return_value;
	}
	if(psi_){
		if(pointer_stack_int[position_in_stack] == nullptr){
			printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
			return return_value;
		}
		return std::to_string(*pointer_stack_int[position_in_stack]);
	}else if(pss_){
		if(pointer_stack_string[position_in_stack] == nullptr){
			printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
			return return_value;
		}
		return std::to_string(*pointer_stack_int[position_in_stack]);
	}else if(psld_){
		if(pointer_stack_long_double[position_in_stack] == nullptr){
			printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
			return return_value;
		}
		return std::to_string(*pointer_stack_int[position_in_stack]);
	}else if(os_){
		return out_stack[position_in_stack];
	}else if(is_){
		return in_stack[position_in_stack];
	}
	return return_value;
}

int run_command(){
	if(command[0] == "modify"){
		if(command[1] == "bitwise"){
			int_ var[2];
			if(in_stack[1] == "" && command[2] == "not"){
				in_stack[1] = "0"; // not
			}
			try{
				var[0] = std::stoll(string_to_decimal_string(in_stack[0]));
				var[1] = std::stoll(string_to_decimal_string(in_stack[1]));
			}catch(std::invalid_argument){
				printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
			if(command[2] == "xor"){
				out_stack[0] = std::to_string(var[0]^var[1]);
			}else if(command[2] == "or"){
				out_stack[0] = std::to_string(var[0]|var[1]);
			}else if(command[2] == "and"){
				out_stack[0] = std::to_string(var[0]&var[1]);
			}else if(command[2] == "not"){
				out_stack[0] = std::to_string(~var[0]);
			}else if(command[2] == "left_shift"){
				out_stack[0] = std::to_string(var[0] << var[1]);
			}else if(command[2] == "right_shift"){
				out_stack[0] = std::to_string(var[0] >> var[1]);
			}else{
				return -1;
			}
		}else if(command[1] == "math"){
			int_ var[2];
			if(in_stack[1] == "" && command[2] == "sqrt"){
				in_stack[1] = "0"; // sqrt
			}
			try{
				var[0] = std::stoll(string_to_decimal_string(in_stack[0]));
				var[1] = std::stoll(string_to_decimal_string(in_stack[1]));
			}catch(std::invalid_argument){
				printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
			if(command[2] == "mul"){
				out_stack[0] = std::to_string(var[0]*var[1]);
			}else if(command[2] == "div"){
				out_stack[0] = std::to_string(var[0]/var[1]);
			}else if(command[2] == "add"){
				out_stack[0] = std::to_string(var[0]+var[1]);
			}else if(command[2] == "sub"){
				out_stack[0] = std::to_string(var[0]-var[1]);
			}else if(command[2] == "pow"){
				out_stack[0] = std::to_string(pow(var[0], var[1]));
			}else if(command[2] == "sqrt"){
				out_stack[0] = std::to_string(sqrt(var[0]));
			}else{
				return -1;
			}
		}
	}else if(command[0] == "set"){ // set
		/*
		  set:
		  in_stack[0]: array id of the desired variable
		  in_stack[1]: the data type (long double, int_, std::string, etc...)
		  in_stack[2]: the entry in this vector
		  in_stack[3]: the target value
		 */
		const std::string array_id_ = in_stack[0];
		const std::string set_type_ = in_stack[1];
		const std::string entry_ = in_stack[2];
		const std::string target_val_ = in_stack[3];
		array_id_t array_id;
		int_ entry;
		try{
			array_id = std::stoll(array_id_);
			entry = std::stoll(entry_);
		}catch(std::invalid_argument){
			printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
			return -1;
		}
		array_t *array_ = find_array_pointer(array_id);
		if(array_ == nullptr){
			printf_(INVALID_OBJECT_ERROR, PRINTF_ERROR);
			return -1;
		}
		if(set_type_ == "int"){
			if(array_->int_array.size() <= entry){
				printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
				return -1;
			}
			try{
				*array_->int_array[entry] = std::stoll(get_target_value(target_val_, array_->int_array[entry], "int"));
			}catch(std::invalid_argument){
				printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
		}else if(set_type_ == "long_double"){
			if(array_->long_double_array.size() <= entry){
				printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
				return -1;
			}
			try{
				*array_->long_double_array[entry] = std::stold(get_target_value(target_val_, array_->long_double_array[entry], "long_double"));
			}catch(std::invalid_argument){
				printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
		}else if(set_type_ == "string"){
			if(array_->string_array.size() <= entry){
				printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
				return -1;
			}
			try{
				*array_->string_array[entry] = get_target_value(target_val_, array_->string_array[entry], "string");
			}catch(std::invalid_argument){
				printf_(STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
		}else{
			return -1;
		}
	}else if(std::isdigit(command[0][0])){
		// 0-7 STACK VALUE
		int_ in_stack_entry = std::stoll(command[0]);
		if(in_stack_entry < 0 && in_stack_entry > 8){
			printf_(OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
			return -1;
		}
		if(command[1] == "pointer_int_stack"){
			pointer_stack_int[in_stack_entry] = (int_*)last_used_pointer;
		}else if(command[1] == "pointer_long_double_stack"){
			pointer_stack_long_double[in_stack_entry] = (long double*)last_used_pointer;
		}else if(command[1] == "pointer_string_stack"){
			pointer_stack_string[in_stack_entry] = (std::string*)last_used_pointer;
		}else if(command[1] == "in_stack"){
			in_stack[in_stack_entry] = get_target_value(command[2], &in_stack[in_stack_entry], "string");
		}else if(command[1] == "out_stack"){
			out_stack[in_stack_entry] = get_target_value(command[2], &out_stack[in_stack_entry], "string");
		}else{
			return -1;
		}
		/*
		  if you want to refer to an item
		  inside of the pointer_stack_int, 
		  you would put 3_PSI to refer to
		  the third item. PSLD and PSS also
		  work
		 */
	}else if(command[0] == "stack_shift"){
		std::string *stack_ = nullptr;
		int_* stack_int_ = nullptr;
		long double* stack_long_double_= nullptr;
		std::string stack_string_ = nullptr;
		if(command[1] == "out_stack"){
			stack_ = out_stack;
		}else if(command[1] == "in_stack"){
			stack_ = in_stack;
		}else{
			printf_(NOT_IMPLEMENTED_ERROR, PRINTF_ERROR);
		}
		if(stack_ != nullptr){
			if(command[2] == "left"){
				for(unsigned long int i = 0;i < 6;i++){
					stack_[i] = stack_[i+1];
				}
				stack_[7] = "";
			}else if(command[2] == "right"){
				for(int i = 6;i >= 0;i--){
					stack_[i+1] = stack_[i];
				}
				stack_[0] = "";
			}else{
				return -1;
			}
		}
	}else if(command[0] == "print"){
		if(command[1] == "last_used_pointer"){
			if(last_used_pointer == nullptr){
				std::cout << "last_used_pointer: nullptr" << std::endl;
			}else{
				std::cout << "last_used_pointer: " << last_used_pointer << std::endl;
			}
		}else if(command[1] == "stacks"){
			std::string output;
			for(unsigned long int i = 0;i < 8;i++){
				output += "in_stack[" + std::to_string(i) + "]: " + in_stack[i] + "\n";
			}
			for(unsigned long int i = 0;i < 8;i++){
				output += "out_stack[" + std::to_string(i) + "]: " + out_stack[i] + "\n";
			}
			for(unsigned long int i = 0;i < 8;i++){
				output += "pointer_stack_int[" + std::to_string(i) + "]: " + ((pointer_stack_int[i] == nullptr) ? "nullptr" : std::to_string(*pointer_stack_int[i])) + "\n";
			}
			for(unsigned long int i = 0;i < 8;i++){
				output += "pointer_stack_long_double[" + std::to_string(i) + "]: " + ((pointer_stack_long_double[i] == nullptr) ? "nullptr" : std::to_string(*pointer_stack_long_double[i])) + "\n";
			}
			for(unsigned long int i = 0;i < 8;i++){
				output += "pointer_stack_string[" + std::to_string(i) + "]: " + ((pointer_stack_string[i] == nullptr) ? "nullptr" : *pointer_stack_string[i]) + "\n";
			}
			std::cout << output << std::endl;
		}else if(command[1] == "array_vector"){
			for(uint_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
				if(array_vector[i] != nullptr){
					std::cout << array_vector[i]->print() << std::endl;
				}
			}
		}else if(command[1] == "locked_arrays"){
			for(uint_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
				if(array_vector[i] != nullptr){
					if(array_vector[i]->unlocked() == false){
						std::cout << "array_vector entry: " << i << std::endl;
					}else{
						array_vector[i]->data_lock.unlock();
					}
				}
			}
		}else{
			return -1;
		}
	}else if(command[0] == "test"){
		if(command[1] == "net"){
			if(net != nullptr){
				net_ip_connection_info_t *tmp = new net_ip_connection_info_t;
				tmp->ip = "127.0.0.1";
				tmp->port = NET_IP_SERVER_RECEIVE_PORT;
				net->write("temp", 0, tmp->array.id);
				for(int i = 0;i < 8;i++) net->loop();
				std::string read = net->read();
				if(read == "temp"){
					std::cout << "Test was successful with four bytes" << std::endl;
				}else{
					std::cout << "Test failed. Received '" << read << "'" << std::endl;
				}
				delete tmp;
				tmp = nullptr;
			}
		}
	}else if(command[0] == "quit"){
		set_signal(SIGTERM, true);
	}else{
		return -1;
	}
	return 0;
}
