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

static std::thread *console_thread = nullptr;
static std::string line;
static std::string in_stack[STACK_SIZE];
static std::string out_stack[STACK_SIZE];
static int_* pointer_stack_int[STACK_SIZE] = {nullptr};
static long double* pointer_stack_long_double[STACK_SIZE] = {nullptr};
static std::string* pointer_stack_string[STACK_SIZE] = {nullptr};
static void* pointer_stack_void[STACK_SIZE] = {nullptr};

static uint_ recursion_count = 0;

void blank_commands(std::string *commands){
	for(uint_ i = 0;i < STACK_SIZE;i++){
		if(commands[i] == ""){
			return;
		}else{
			commands[i] = "";
		}
	}
}

void convert_line_to_commands(std::string command_, std::string *command){
	blank_commands(command);
	std::stringstream ss(command_);
	ss >> command[0] >> command[1] >> command[2] >> command[3] >> command[4] >> command[5] >> command[6] >> command[7];
	if(command[0] == "done" && command[1] == "sleep") assert(false);
	for(uint_ i = 0;i < 8;i++){ // should work well enough (no tabs after the first char)
		while(command[i][0] == '\t' || command[i][0] == '\n'){
			command[i].erase(command[i].begin());
		}
	}
}

shell_script_t::shell_script_t() : array(this, "shell_script_t", ARRAY_SETTING_IMMUNITY){
	array.string_array.push_back(std::make_pair(&name, "name"));
}

int run_command(std::string*);

static bool valid_command_line(std::string *command_line){
	return (*command_line)[0] != '#' && (*command_line)[0] != '\n' && (*command_line)[0] != '\r' && (*command_line)[0] != ' ';
}

static uint_ detect_end_of_conditional_statement(std::vector<std::string> *pointer_to_vector, uint_ start){
	uint_ place_in_stack = 0;
	std::string current_commands[STACK_SIZE];
	for(uint i = start;i < pointer_to_vector->size();i++){
		if(valid_command_line(&(*pointer_to_vector)[i])){
			convert_line_to_commands((*pointer_to_vector)[i], current_commands);
			if(current_commands[0] == "if" || current_commands[0] == "while"){
				place_in_stack++;
			}else if(current_commands[0] == "done"){
				place_in_stack--;
			}
			if(place_in_stack == 0){
				return i;
			}
		}
	}
	printf_("ERROR: detect_end_of_conditional_statement: Couldn't detect the end of the conditonal, there is probably a syntax error in the script\n", PRINTF_ERROR);
	throw std::logic_error("script syntax error");
	return 0;
}

static uint_ run_conditional_statement(std::vector<std::string> *pointer_to_vector, uint_ start){
	const uint_ end = detect_end_of_conditional_statement(pointer_to_vector, start); // return done slot, the iterator +1s it
	printf_("SPAM: run_conditional_statement: end of the current statement is line #" + std::to_string(end) + "\n", PRINTF_SPAM);
	std::string current_commands[STACK_SIZE];
	std::string conditional_commands[STACK_SIZE];
	convert_line_to_commands((*pointer_to_vector)[start], conditional_commands);
	bool loop_code;
	beginning_of_conditional_statement:
	do{
		if(run_command(conditional_commands) != 0){
			throw std::logic_error("syntax error");
		}
		loop_code = in_stack[CONDITION_STACK_ENTRY] != "0" && conditional_commands[0] == "while";
		if(in_stack[CONDITION_STACK_ENTRY] == "0"){
			printf_("SPAM: Conditional returned false, not running the code\n", PRINTF_SPAM);
			return end;
		}else{
			for(uint_ i = start+1;i < end;i++){
				if(valid_command_line(&(*pointer_to_vector)[i])){
					convert_line_to_commands((*pointer_to_vector)[i], current_commands);
					if(current_commands[0] == "if" || current_commands[0] == "while"){
						printf_("SPAM: Found recursive conditionals\n", PRINTF_DEBUG);
						run_conditional_statement(pointer_to_vector, i);
					}else if(run_command(current_commands) != 0){
						throw std::logic_error("syntax error");
					}
				}
			}
		}
	}while(loop_code);
	printf_("DEBUG: loop_code is false, not running the code again (" + in_stack[CONDITION_STACK_ENTRY] + " " + conditional_commands[0] + "\n", PRINTF_DEBUG);
	return end;
}

int_ shell_script_t::run(){
	int_ retval = 0;
	recursion_count++;
	if(recursion_count > MAX_RECURSION_COUNT){
		printf_("ERROR: The MAX_RECURSION_COUNT has been reached, aborting this script to prevent a crash\n", PRINTF_ERROR);
	}else{
		printf_("SPAM: shell_script_t::run: Starting a shell script\n", PRINTF_SPAM);
		int_ level_of_statements = 0;
		for(uint_ i = 0;i < commands.size();i++){
			if(commands[i].size() != 0 && commands[i][0] != '#' && commands[i][0] != ' ' && commands[i][0] != '\n'){
				std::string command_array[STACK_SIZE];
				convert_line_to_commands(commands[i], command_array);
				if(command_array[0] == "if" || command_array[0] == "while"){
					printf_("SPAM: shell_script_t::run: found a conditional, running the special run_conditional_statement function\n", PRINTF_SPAM);
					uint_ i_ = i;
					i = run_conditional_statement(&commands, i);
					printf_("SPAM: shell_script_t::run: old i: " + std::to_string(i_) + " new i: " + std::to_string(i) + "\n", PRINTF_SPAM);
				}else if(run_command(command_array) != 0){
					printf_("ERROR: shell_script_t::run: run_command on a shell script returned -1. Aborting the entire shell script.\n", PRINTF_ERROR);
					retval = -1;
				}else{
					printf_("SPAM: shell_script_t::run: run_command ran successfully\n", PRINTF_SPAM);
				}
			}
		}
	}
	recursion_count--;
	return 0;
}

shell_script_t::~shell_script_t(){}

function_t::function_t(void* function_, std::string name_, int_ settings_) : array(this, "function_t", ARRAY_SETTING_IMMUNITY){
	array.data_type = "function_t";
	function = function_;
	name = name_;
	settings = settings_;
	array.string_array.push_back(std::make_pair(&return_value, "function return value"));
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
			return_value = std::to_string(((int_(*)(int_))function)(int_value[0]));
		}else if((settings & FUNCTION_PARAMETER_VOID) != 0){
			return_value = std::to_string(((int_(*)(void))function)());
		}else if((settings & FUNCTION_PARAMETER_VOID_PTR) != 0){
			return_value = std::to_string(((int_(*)(void*))function)(void_ptr_value[0]));
		}else if((settings & FUNCTION_PARAMETER_STRING) != 0){
			return_value = std::to_string(((int_(*)(std::string))function)(string_value[0]));
		}else return -1;
		return 0;
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

int_ check_if_id_exists(array_id_t id){
	return find_array_pointer(id) != nullptr;
}

void load_shell_script(std::string data_){
	std::ifstream script_fd(data_);
	if(script_fd.is_open()){
		std::string data;
		shell_script_t *tmp = new shell_script_t;
		tmp->name = data_.substr(data_.find_last_of("/")+1, data_.size());
		while(std::getline(script_fd, data)){
			tmp->commands.push_back(data);
		}
		script_fd.close();
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
	std::string data_ = "";
	while(std::getline(in, data_)){
		load_shell_script(data_);
	}
	in.close();
}

void console_init(){
	console_thread = new std::thread(console_engine);
	new function_t((void*)check_if_id_exists, "check_if_id_exists", FUNCTION_RETURN_INT_ | FUNCTION_PARAMETER_INT_);
	load_all_shell_scripts();
}

void console_engine(){
	while(true){
		std::cout << "]";
		std::string command[STACK_SIZE];
		getline(std::cin, command[0]);
		convert_line_to_commands(command[0], command);
		long double start_time = get_time();
		int_ run_command_retval = run_command(command);
		if(run_command_retval == 0){
			std::cout << "run_command finished in " << get_time()-start_time << "s" << std::endl;
		}else if(run_command_retval == 1){
			return;
		}else{
			std::cout << "run_command failed" << std::endl;
		}
	}
}

void console_close(){
	console_thread->join();
	delete console_thread;
	console_thread = nullptr;
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
	//	std::cout << "get_target_value: " << target_value << " " << thing << " " << type << std::endl;
	}
	const bool psi_ = target_value.find("_PSI") != std::string::npos;
	const bool pss_ = target_value.find("_PSS") != std::string::npos;
	const bool psld_ = target_value.find("_PSLD") != std::string::npos;
	const bool os_ = target_value.find("_OS") != std::string::npos;
	const bool is_ = target_value.find("_IS") != std::string::npos;
	bool nil_ = target_value == "NIL";
	if(check_for_parameter("--debug", argc_, argv_)){
	//	std::cout << target_value << std::endl;
	//	std::cout << psi_ << pss_ << psld_ << os_ << is_ << nil_ << std::endl;
	}
	last_used_pointer = thing;
	if(target_value == "" && type != "string"){
		nil_ = true;
	}
	if(!psi_ && !pss_ && !psld_ && !os_ && !is_ && !nil_){
	//	printf_("DEBUG: No special conditions have been met, returning the target_value by itself ('" + target_value + "')\n", PRINTF_DEBUG);
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
		//printf_("DEBUG: Detected NIL, returning itself ('" + return_value + "')\n", PRINTF_DEBUG);
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
		return in_stack[position_in_stack];
	}
	return return_value;
}

static std::string convert_commands_to_line(std::string *commands){
	std::string retval;
	for(uint_ i = 0;i < STACK_SIZE;i++){
		if(commands[i] == ""){
			return retval;
		}
		retval += commands[i] + " ";
	}
	return retval;
}

int run_command(std::string* command){
	if(command[0] == "while"){ // loose typing, should do this another way
		command[0] = "if";
		int a = run_command(command);
		command[0] = "while";
		return a;
	}
	if(valid_command_line(&(command[0])) == false){
		/*
		^
		|
		That should take the entire line, not just the first entry. 
		However, I don't feel like converting it, so it shall stay like this.
		*/
		printf_("WARNING: run_command: caught a non-command, this shouldn't have been reported\n", PRINTF_UNLIKELY_WARN);
		return -1;
	}
	printf_("DEBUG: run_command: command == " + convert_commands_to_line(command) + "\n", PRINTF_DEBUG);
	if(command[0] == "modify"){
		if(command[1] == "bitwise"){
			int_ var[2];
			if(command[4] == "" && command[2] == "not"){
				command[4] = "0"; // not
			}
			try{
				var[0] = std::stoll(string_to_decimal_string(get_target_value(command[3], &command[3], "string")));
				var[1] = std::stoll(string_to_decimal_string(get_target_value(command[4], &command[4], "string")));
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
				var[0] = std::stoll(string_to_decimal_string(get_target_value(command[3], &command[3], "string")));
				var[1] = std::stoll(string_to_decimal_string(get_target_value(command[4], &command[4], "string")));
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
		  command[1]: array id of the desired variable
		  command[2]: the data type (long double, int_, std::string, etc...)
		  command[3]: the entry in this vector
		  command[4]: the target value
		 */
		const std::string array_id_ = command[1];
		const std::string set_type_ = command[2];
		const std::string entry_ = command[3];
		const std::string target_val_ = command[4];
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
			if(command[2] == "in_stack"){
				for(unsigned long int i = 0;i < WORKING_STACK_SIZE;i++){
					output += "in_stack[" + std::to_string(i) + "]: " + in_stack[i] + "\n";
				}
			}else if(command[2] == "out_stack"){
				for(unsigned long int i = 0;i < WORKING_STACK_SIZE;i++){
					output += "out_stack[" + std::to_string(i) + "]: " + out_stack[i] + "\n";
				}
			}else if(command[2] == "pointer_stack_int"){
				for(unsigned long int i = 0;i < WORKING_STACK_SIZE;i++){
					output += "pointer_stack_int[" + std::to_string(i) + "]: " + ((pointer_stack_int[i] == nullptr) ? "nullptr" : std::to_string(*pointer_stack_int[i])) + "\n";
				}
			}else if(command[2] == "pointer_stack_long_double"){
				for(unsigned long int i = 0;i < WORKING_STACK_SIZE;i++){
					output += "pointer_stack_long_double[" + std::to_string(i) + "]: " + ((pointer_stack_long_double[i] == nullptr) ? "nullptr" : std::to_string(*pointer_stack_long_double[i])) + "\n";
				}
			}else if(command[2] == "pointer_stack_string"){
				for(unsigned long int i = 0;i < WORKING_STACK_SIZE;i++){
					output += "pointer_stack_string[" + std::to_string(i) + "]: " + ((pointer_stack_string[i] == nullptr) ? "nullptr" : *pointer_stack_string[i]) + "\n";
				}
			}else{
				return -1;
			}
			std::cout << output << std::endl;
		}else if(command[1] == "variable"){
			printf_(get_target_value(command[2], &command[2], "string") + "\n", PRINTF_SCRIPT);
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
			net_t *net_ = new net_t; // should automatically use whatever was set in the argv (the IP engine)
			net_ip_connection_info_t *inbound_info = new net_ip_connection_info_t;
			inbound_info->ip = "127.0.0.1";
			inbound_info->port = 50005;
			net_->set_inbound_info(inbound_info->array.id);
			net_ip_connection_info_t *tmp = new net_ip_connection_info_t;
			tmp->ip = "127.0.0.1";
			tmp->port = 50005; // this net operates on a different port, so there should be no conflict (except with the outbound port 0, but that just means any port can be selected)
			net_->write("temp", 0, tmp->array.id);
			for(int i = 0;i < 5000;i++){
				update_progress_bar(i/5000.0, "net test");
				net_->loop();
				std::string read = net_->read("temp");
				if(read == "temp"){
					std::cout << "Test was successful with four bytes" << std::endl;
					break;
				}else if(read != ""){
					std::cout << "Test failed. Received '" << read << "'" << std::endl;
				}
				ms_sleep(1);
			}
			update_progress_bar(1, "net test");
			delete net_;
			net_ = nullptr;
			delete tmp;
			tmp = nullptr;
			delete inbound_info;
			inbound_info = nullptr;
		}else if(command[2] == "server_connection"){
			try{
				server_time_t *time_ = (server_time_t*)find_pointer(all_ids_of_type("server_time_t")[0]);
				throw_if_nullptr(time_);
				printf_("server_time_t->get_timestamp() returned " + std::to_string(time_->get_timestamp()) + "\n", PRINTF_SCRIPT);
				printf_("time(NULL) returned " + std::to_string(time(NULL)) + "\n", PRINTF_SCRIPT);
			}catch(std::logic_error &e){
				printf_("There is no server connection\n", PRINTF_SCRIPT);
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
					printf_("script _retval: " + std::to_string(_retval) + "\n", PRINTF_FORCE_PRINT);;
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
		return 1;
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
		}else return -1;
	}else if(command[0] == "do"){
		if(in_stack[CONDITION_STACK_ENTRY] != "0"){ // Should I use != "0" or == "1"?
			run_command(&command[1]);
		} // don't return in this instance
	}else if(command[0] == "done"){
	}else{
		return -1;
	}
	return 0;
}
