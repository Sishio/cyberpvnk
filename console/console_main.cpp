#include "../input/input_main.h" // signals
#include "../class/class_array.h"
#include "../util/util_main.h"
#include "../net/net_main.h" // testing
#include "console_main.h"

#define STACK_SIZE 8192 // because why not?
/*
  But seriously, if there is a time
  when scripts call each other for more
  complicated functions, it would be
  nice to not run out of space on the stack.
  Also, this takes up virtually no space, so
  that is always nice
 */
#define WORKING_STACK_SIZE 32
#define CONDITION_STACK_ENTRY WORKING_STACK_SIZE-1

static std::thread console_thread;
static std::string line;
static std::string in_stack[STACK_SIZE];
static std::string out_stack[STACK_SIZE];
static int_* pointer_stack_int[STACK_SIZE] = {nullptr};
static long double* pointer_stack_long_double[STACK_SIZE] = {nullptr};
static std::string* pointer_stack_string[STACK_SIZE] = {nullptr};
static void* pointer_stack_void[STACK_SIZE] = {nullptr};

void convert_line_to_commands(std::string command_, std::string *command){
	std::stringstream ss(command_);
	ss >> command[0] >> command[1] >> command[2] >> command[3] >> command[4] >> command[5] >> command[6] >> command[7];
	printf_("DEBUG: console command: " + command[0] + " " + command[1] + " " + command[2] + " " + command[3] + " " + command[4] + " " + command[5] + " " + command[6] + " " + command[7] + "\n", PRINTF_DEBUG);
	for(uint_ i = 0;i < 8;i++){ // should work well enough (no tabs after the first char)
		while(command[i][0] == '\t' || command[i][0] == '\n'){
			command[i].erase(command[i].begin());
		}
	}
}


struct shell_script_t{
public:
	shell_script_t();
	~shell_script_t();
	array_t array;
	std::vector<std::string> commands; // convert this to the commands when it is used to simplify editing
	std::string name;
	int_ run();
};

shell_script_t::shell_script_t() : array(this, "shell_script_t", ARRAY_SETTING_IMMUNITY){
	array.string_array.push_back(std::make_pair(&name, "name"));
}

int run_command(std::string*);

int_ shell_script_t::run(){
	printf_("DEBUG: Starting a shell script\n", PRINTF_DEBUG);
	int_ level_of_statements = 0;
	for(uint_ i = 0;i < commands.size();i++){
		if(commands[i].size() != 0 && commands[i][0] != '#' && commands[i][0] != ' ' && commands[i][0] != '\n'){
			std::string command[STACK_SIZE];
			convert_line_to_commands(commands[i], command);
			if(command[0] == "if" || command[0] == "while"){
				int_ level_of_statements_ = level_of_statements; // the following code re-reads the first line and processes the whole chunk
				uint_ c;
				for(c = i;c < commands.size();c++){
					std::string command_[STACK_SIZE];
					convert_line_to_commands(commands[i], command_);
					if(command_[0] == "if" || command_[0] == "while"){
						level_of_statements_++;
					}else if(command_[0] == "done"){
						level_of_statements_--;
					}
					if(level_of_statements_ == level_of_statements){
						if(command[0] == "if"){
							break;
						}else if(command[0] == "while"){
							std::string *command_ptr = nullptr;
							// changes while to 'if' if 'if' isn't following while
							if(command[1] == "if"){
								command_ptr = &command[1];
							}else{
								command[0] = "if";
								command_ptr = &command[0];
							}
							run_command(command_ptr);
							if(in_stack[CONDITION_STACK_ENTRY] == "1"){
								c = i;
							}else if(in_stack[CONDITION_STACK_ENTRY] == "0"){
								break;
							}
						}
					}
				}
				i = c; // skip to the end of this complicated bit
			}else if(run_command(command) == -1){
				printf_("ERROR: run_command on a shell script returned -1. Aborting the entire shell script.\n", PRINTF_ERROR);
				return -1;
			}else{
				printf_("DEBUG: run_command ran successfully\n", PRINTF_DEBUG);
			}
		}
	}
	return 0;
}

shell_script_t::~shell_script_t(){}

struct function_t{
public:
	std::string name;
	void* function;
	array_t array;
	function_t(void*, std::string, int_);
	~function_t();
	bool functions_equal(void *a){return a == function;}
	std::string get_name();
	int_ run();
	int_ settings;
	int_ int_value[8];
	void * void_ptr_value[8];
	std::string string_value[8];
};

function_t::function_t(void* function_, std::string name_, int_ settings_) : array(this, "function_t", ARRAY_SETTING_IMMUNITY){
	array.data_type = "function_t";
	function = function_;
	name = name_;
	settings = settings_;
	array.string_array.push_back(std::make_pair(&name, "function name"));
	for(uint_ i = 0;i < 8;i++){
		int_value[i] = DEFAULT_INT_VALUE;
		array.int_array.push_back(std::make_pair(&int_value[i], "function int parameter " + std::to_string(i)));
		void_ptr_value[i] = nullptr;
		array.void_ptr_array.push_back(std::make_pair(&void_ptr_value[i], "functoin void ptr parameter " + std::to_string(i)));
		array.string_array.push_back(std::make_pair(&string_value[i], "function string parameter " + std::to_string(i)));
	}
}

std::string function_t::get_name(){
	return name;
}

int_ function_t::run(){
	if(function == nullptr){
		printf_("ERROR: function_t::run(): Cannot run a null function\n", PRINTF_ERROR);
		return -1;
	}
	if(settings == 0){
		printf_("ERROR: function_t::run(): settings == 0\n", PRINTF_ERROR);
		return -1;
	}
	if((settings & FUNCTION_RETURN_VOID) != 0){
		if((settings & FUNCTION_PARAMETER_INT_) != 0){
			((void(*)(int_))function)(int_value[0]);
		}else if((settings & FUNCTION_PARAMETER_VOID) != 0){
			((void(*)(void))function)();
		}else if((settings & FUNCTION_PARAMETER_VOID_PTR) != 0){
			((void(*)(void*))function)(void_ptr_value[0]);
		}else if((settings & FUNCTION_PARAMETER_STRING) != 0){
			((void(*)(std::string))function)(string_value[0]);
		}else return -1;
		return 0;
	}else if((settings & FUNCTION_RETURN_INT_) != 0){
		if((settings & FUNCTION_PARAMETER_INT_) != 0){
			return ((int_(*)(int_))function)(int_value[0]);
		}else if((settings & FUNCTION_PARAMETER_VOID) != 0){
			return ((int_(*)(void))function)();
		}else if((settings & FUNCTION_PARAMETER_VOID_PTR) != 0){
			return ((int_(*)(void*))function)(void_ptr_value[0]);
		}else if((settings & FUNCTION_PARAMETER_STRING) != 0){
			return ((int_(*)(std::string))function)(string_value[0]);
		}else return -1;
	}else return -1;
}

function_t::~function_t(){
	function = nullptr;
}

function_t *function_get(std::string a){
	std::vector<array_id_t> function_vector = all_ids_of_type("function_t");
	function_t *retval = nullptr;
	uint_ function_vector_size = function_vector.size();
	for(uint_ i = 0;i < function_vector_size;i++){
		function_t *tmp = (function_t*)find_pointer(function_vector[i]);
		if(tmp != nullptr){
			tmp->array.data_lock.lock();
			if(tmp->get_name() == a){
				retval = tmp;
				tmp->array.data_lock.unlock();
				break;
			}
		}
	}
	return retval;
}

/*
  I would LOVE to use the void* stack, but
  making casts and calls to the items I would
  use it for would be real silly, unless I print
  all of the functions it uses and make a void(*)
  to the function inside of the class and run
  it like that, but that just sounds complicated and
  the amount of checks I would have to put in there
  to make sure this doesn't crash the program would be
  something different.

  I guess I could put a little function inside the array_t
  that puts the pointer to the main item in some main array
  and the size of the item. Of course, this will only work
  on the main object and not the objects that are inside of it
  (unless I make a pointer array alongside the other arrays
  that keeps tabs on that). Regardless, this is adding a bit
  of stability but the ID lookup method is still faster than
  this (for the most part).
 */

static void *last_used_pointer = nullptr;

#define STD_INVALID_ARGUMENT_ERROR	"An error was reported: std::invalid_argument\n"
#define OUT_OF_BOUNDS_ERROR		"An error was reported: you have requested an out-of-bounds item\n"
#define INVALID_OBJECT_ERROR		"An error was reported: the object you requested doesn't exist\n"
#define NOT_IMPLEMENTED_ERROR		"An error was reported: the command you have given has not been implemented yet\n"

void load_shell_script(std::string data_){
	std::ifstream script_fd(data_);
	if(script_fd.is_open()){
		std::string data;
		shell_script_t *tmp = new shell_script_t;
		tmp->name = data_.substr(data_.find_last_of("/")+1, data_.size());
		while(std::getline(script_fd, data)){
			tmp->commands.push_back(data);
		}
	}else{
		printf_("ERROR: load_shell_script: couldn't open '" + data_ + "'\n", PRINTF_ERROR);
	}
}

static void load_all_shell_scripts(){
	#ifdef __linux
	system("find ../script_data | grep \\.script > script_list");
	#endif
	std::ifstream in("script_list");
	if(in.is_open() == false){ // prob. running windows
		printf_("ERROR: load_shell_scripts: couldn't open script_list\n", PRINTF_ERROR);
		return;
	}
	std::string data_;
	while(std::getline(in, data_)){
		load_shell_script(data_);
	}
}

void console_init(){
	console_thread = std::thread(console_engine);
	load_all_shell_scripts();
	new function_t((void*)load_shell_script, "load shell script", FUNCTION_RETURN_VOID | FUNCTION_PARAMETER_STRING);
}

void console_engine(){
	while(true){
		std::cout << "]";
		std::string command[STACK_SIZE];
		getline(std::cin, command[0]);
		convert_line_to_commands(command[0], command);
		long double start_time = get_time();
		if(run_command(command) == 0){
			std::cout << "run_command finished in " << get_time()-start_time << "s" << std::endl;
		}else{
			std::cout << "run_command failed" << std::endl;
		}
		for(unsigned long int i = 0;i < STACK_SIZE;i++){
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
	if(check_for_parameter("--debug", argc_, argv_)){
		std::cout << "get_target_value: " << target_value << " " << thing << " " << type << std::endl;
	}
	const bool psi_ = target_value.find("_PSI") != std::string::npos;
	const bool pss_ = target_value.find("_PSS") != std::string::npos;
	const bool psld_ = target_value.find("_PSLD") != std::string::npos;
	const bool os_ = target_value.find("_OS") != std::string::npos;
	const bool is_ = target_value.find("_IS") != std::string::npos;
	bool nil_ = target_value == "NIL";
	if(check_for_parameter("--debug", argc_, argv_)){
		std::cout << target_value << std::endl;
		std::cout << psi_ << pss_ << psld_ << os_ << is_ << nil_ << std::endl;
	}
	last_used_pointer = thing;
	if(target_value == "" && type != "string"){
		nil_ = true;
	}
	if(!psi_ && !pss_ && !psld_ && !os_ && !is_ && !nil_){
		printf_("DEBUG: No special conditions have been met, returning the target_value by itself ('" + target_value + "')\n", PRINTF_DEBUG);
		return target_value; // no special conditions have been met
	}
	std::string return_value;
	if(type == "string"){
		return_value = *((std::string*)thing);
	}else if(type == "long_double"){
		return_value = std::to_string(*((long double*)thing));
	}else if(type == "int"){
		std::cout << "INT: " << *((int_*)thing) << std::endl;
		return_value = std::to_string(*((int_*)thing));
	}else{
		assert(false);
	}
	if(nil_){
		printf_("DEBUG: Detected NIL, returning itself ('" + return_value + "')\n", PRINTF_DEBUG);
		return return_value; // worst case scenario: nothing changes
	}
	int_ position_in_stack;
	try{
		position_in_stack = std::stoll(target_value);
	}catch(std::invalid_argument){
		printf_((std::string)"get_target_value: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
		return return_value;
	}
	if(position_in_stack > STACK_SIZE && position_in_stack < 0){
		printf_((std::string)"get_target_value: " + OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
		return return_value;
	}
	if(psi_){
		if(pointer_stack_int[position_in_stack] == nullptr){
			printf_((std::string)"get_target_value: " + INVALID_OBJECT_ERROR, PRINTF_ERROR);
			return return_value;
		}
		return std::to_string(*pointer_stack_int[position_in_stack]);
	}else if(pss_){
		if(pointer_stack_string[position_in_stack] == nullptr){
			printf_((std::string)"get_target_value: " + INVALID_OBJECT_ERROR, PRINTF_ERROR);
			return return_value;
		}
		return std::to_string(*pointer_stack_int[position_in_stack]);
	}else if(psld_){
		if(pointer_stack_long_double[position_in_stack] == nullptr){
			printf_((std::string)"get_target_value: " + INVALID_OBJECT_ERROR, PRINTF_ERROR);
			return return_value;
		}
		return std::to_string(*pointer_stack_int[position_in_stack]);
	}else if(os_){
		return out_stack[position_in_stack];
	}else if(is_){
		printf_("DEBUG: Detected _IS, returning '" + in_stack[position_in_stack] + "'\n", PRINTF_DEBUG);
		return in_stack[position_in_stack];
	}
	return return_value;
}

int run_command(std::string* command){
	assert(command[0] != "done" || command[0] != "while");
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
				printf_((std::string)"run_command: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
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
				printf_((std::string)"run_command: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
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
		}else{
			return -1;
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
			printf_((std::string)"run_command: " + (std::string)(STD_INVALID_ARGUMENT_ERROR) + " with the array_id and the entry", PRINTF_ERROR);
			return -1;
		}
		array_t *array_ = find_array_pointer(array_id);
		if(array_ == nullptr){
			printf_((std::string)"run_command: " + INVALID_OBJECT_ERROR, PRINTF_ERROR);
			return -1;
		}
		if(set_type_ == "int"){
			if(array_->int_array.size() <= entry){
				printf_((std::string)"run_command: " + OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
				return -1;
			}
			try{
				*(std::get<0>(array_->int_array[entry])) = std::stoll(get_target_value(target_val_, std::get<0>(array_->int_array[entry]), "int"));
			}catch(std::invalid_argument){
				printf_((std::string)"run_command: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
		}else if(set_type_ == "long_double"){
			if(array_->long_double_array.size() <= entry){
				printf_((std::string)"run_command: " + OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
				return -1;
			}
			try{
				*(std::get<0>(array_->long_double_array[entry])) = std::stold(get_target_value(target_val_, std::get<0>(array_->long_double_array[entry]), "long_double"));
			}catch(std::invalid_argument){
				printf_((std::string)"run_command: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
		}else if(set_type_ == "string"){
			if(array_->string_array.size() <= entry){
				printf_((std::string)"run_command: " + OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
				return -1;
			}
			try{
				*(std::get<0>(array_->string_array[entry])) = get_target_value(target_val_, std::get<0>(array_->string_array[entry]), "string");
			}catch(std::invalid_argument){
				printf_((std::string)"run_command: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
				return -1;
			}
		}else{
			return -1;
		}
	}else if(std::isdigit(command[0][0])){
		// 0-7 STACK VALUE
		int_ in_stack_entry = std::stoll(command[0]);
		if(in_stack_entry < 0 && in_stack_entry > STACK_SIZE){
			printf_((std::string)"run_command: " + OUT_OF_BOUNDS_ERROR, PRINTF_ERROR);
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
		if(command[1] == "out_stack"){
			stack_ = out_stack;
		}else if(command[1] == "in_stack"){
			stack_ = in_stack;
		}else{
			printf_((std::string)"run_command: " + NOT_IMPLEMENTED_ERROR, PRINTF_ERROR);
		}
		if(stack_ != nullptr){
			if(command[2] == "left"){
				for(uint_ c = 0;c < WORKING_STACK_SIZE;c++){
					for(unsigned long int i = 0;i < STACK_SIZE-2;i++){
						stack_[i] = stack_[i+1];
					}
				}
				stack_[STACK_SIZE-1] = "";
			}else if(command[2] == "right"){
				for(uint_ c = 0;c < WORKING_STACK_SIZE;c++){
					for(int i = STACK_SIZE-2;i >= 0;i--){
						stack_[i+1] = stack_[i];
					}
				}
				stack_[0] = "";
			}else{
				return -1;
			}
		}else{
			return -1;
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
			for(unsigned long int i = 0;i < STACK_SIZE;i++){
				output += "in_stack[" + std::to_string(i) + "]: " + in_stack[i] + "\n";
			}
			for(unsigned long int i = 0;i < STACK_SIZE;i++){
				output += "out_stack[" + std::to_string(i) + "]: " + out_stack[i] + "\n";
			}
			for(unsigned long int i = 0;i < STACK_SIZE;i++){
				output += "pointer_stack_int[" + std::to_string(i) + "]: " + ((pointer_stack_int[i] == nullptr) ? "nullptr" : std::to_string(*pointer_stack_int[i])) + "\n";
			}
			for(unsigned long int i = 0;i < STACK_SIZE;i++){
				output += "pointer_stack_long_double[" + std::to_string(i) + "]: " + ((pointer_stack_long_double[i] == nullptr) ? "nullptr" : std::to_string(*pointer_stack_long_double[i])) + "\n";
			}
			for(unsigned long int i = 0;i < STACK_SIZE;i++){
				output += "pointer_stack_string[" + std::to_string(i) + "]: " + ((pointer_stack_string[i] == nullptr) ? "nullptr" : *pointer_stack_string[i]) + "\n";
			}
			std::cout << output << std::endl;
		}else if(command[1] == "array_vector"){
			// this special function works better for this since speed doesn't matter and the command can be blank and still work
			for(uint_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
				if(array_vector[i] != nullptr){
					array_vector[i]->data_lock.lock();
					if(command[2] == "" || command[2] == array_vector[i]->data_type){
						std::cout << array_vector[i]->print() << std::endl;
					}
					array_vector[i]->data_lock.unlock();
				}
			}
		}else if(command[1] == "locked_arrays"){
			for(uint_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
				if(array_vector[i] != nullptr){
					if(array_vector[i]->unlocked() == false){
						std::cout << "array_vector entry: " << i << std::endl;
					}
				}
			}
		}else{
			return -1;
		}
	}else if(command[0] == "test"){
		if(command[1] == "net"){
			std::vector<array_id_t> net_vector = all_ids_of_type("net_t");
			net_t *net = nullptr;
			if(net_vector.size() != 0 && (net = (net_t*)find_pointer(net_vector[0])) != nullptr){
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
			}else{
				return -1;
			}
		}else{
			return -1;
		}
	}else if(command[0] == "signal"){
		int_ signal = -1;
		try{
			signal = std::stoi(command[1]);
		}catch(std::invalid_argument){
			printf_((std::string)"run_command: " + STD_INVALID_ARGUMENT_ERROR, PRINTF_ERROR);
			return -1;
		}
		set_signal(signal, true);
	}else if(command[0] == "function"){
		function_t *function_ = function_get(command[1]);
		if(function_ != nullptr){
			function_->run();
			// All the parameters have to be passed via the set feature.
			// This is to prevent overloading of types and the ability to pass pointers
		}
	}else if(command[0] == "script"){
		std::vector<array_id_t> all_scripts = all_ids_of_type("shell_script_t");
		const uint_ all_scripts_size = all_scripts.size();
		shell_script_t *tmp = nullptr;
		for(uint_ i = 0;i < all_scripts_size;i++){
			tmp = (shell_script_t*)find_pointer(all_scripts[i]);
			if(tmp != nullptr){
				if(tmp->name == command[1]){
					int_ _retval = tmp->run();
					std::cout << "script _retval: " << _retval << std::endl;
					break;
				}
			}
		}
		if(tmp == nullptr){
			printf_("ERROR: run_command: the shell script couldn't be found\n", PRINTF_ERROR);
			return -1;
		}
	}else if(command[0] == "sleep"){
		try{
			ms_sleep(std::stoi(command[1]));
		}catch(std::invalid_argument& e){}
	}else if(command[0] == "quit"){
		set_signal(SIGTERM, true);
	}else if(command[0] == "abort"){
		assert(false);
	}else if(command[0] == "if"){
		if(command[1] == "equal"){
			in_stack[CONDITION_STACK_ENTRY] = std::to_string(get_target_value(command[2], &command[2], "string") == get_target_value(command[3], &command[3], "string"));
		}else if(command[1] == "greater"){
			in_stack[CONDITION_STACK_ENTRY] = std::to_string(get_target_value(command[2], &command[2], "string") == get_target_value(command[3], &command[3], "string"));
		}else if(command[1] == "less"){
			in_stack[CONDITION_STACK_ENTRY] = std::to_string(get_target_value(command[2], &command[2], "string") == get_target_value(command[3], &command[3], "string"));
		}else if(command[1] == "nequal"){
			in_stack[CONDITION_STACK_ENTRY] = std::to_string(get_target_value(command[2], &command[2], "string") != get_target_value(command[3], &command[3], "string"));
		}
	}else{
		return -1;
	}
	return 0;
}
