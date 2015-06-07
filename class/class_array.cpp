/*
Czech_mate by Daniel
This file is part of Czech_mate.

Czech_mate is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as published by
the Free Software Foundation, 

Czech_mate is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Czech_mate.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../loop/loop_struct_forward.h"
#include "../render/render_struct_forward.h"
#include "../net/net_struct_forward.h"
#include "../input/input_struct_forward.h"
#include "../util/util_main.h"
#include "class_main.h"
#include "class_array.h"

//#define CLASS_DEBUG_OUTPUT 1

array_t* array_vector[ARRAY_VECTOR_SIZE] = {nullptr};

#define TYPE_VECTOR_SIZE 64 // only used 13

static std::vector<array_id_t> type_vector[TYPE_VECTOR_SIZE]; // fixed types are neat here

lock_t array_lock;
static std::hash<std::string> type_hash_function;

std::string all_types[TYPE_VECTOR_SIZE] = {
	"coord_t",
	"client_t",
	"model_t",
	"net_ip_connection_info_t",
	"loop_t",
	"input_t",
	"render_t",
	"net_t",
	"input_keyboard_map_t",
	"loop_entry_t",
	"function_t",
	"tile_t",
	"image_t",
	"shell_script_t"
};

int_ type_to_type_entry(std::string a){
	for(uint_ i = 0;i < TYPE_VECTOR_SIZE;i++){
		if(all_types[i] == a){
			return i;
		}
	}
	printf_("WARNING: The requested type doesn't exist in the all_types array yet ('" + a + "'), putting it in there at runtime\n", PRINTF_UNLIKELY_WARN);
	// likely at the time of writing, but this should be reported as UNLIKELY
	for(uint_ i = 0;i < TYPE_VECTOR_SIZE;i++){
		if(all_types[i] == ""){
			all_types[i] = a;
			return i;
		}
	}
	printf_("ERROR: type_to_type_entry: No more room to create a new type (TYPE_VECTOR_SIZE)\n", PRINTF_ERROR);
	throw std::runtime_error(" cannot generate new type");
}

static array_id_t pull_id(std::string a){
	int_ return_value;
	const int_ start = a.find_first_of(ARRAY_ID_START);
	const int_ end = a.find_first_of(ARRAY_ID_END);
	std::string id_string = a.substr(start+1, end-start-1).c_str();
	return_value = atoi(id_string.c_str());
	return return_value;
}

array_id_t array_highest_id(){
	array_id_t id = 0;
	for(int_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		if(likely(array_vector[i]->id > id)){
			id = array_vector[i]->id;
		}
	}
	return id;
}

/*
  WARNING: Make sure everything I do
  to the arrays initially is accounted for
  in the corresponding ARRAY_RESERVE.
 */

int_ find_empty_array_entry(){
	int_ i;
	#ifdef RESERVE_ID_SIZE
	i = RESERVE_ID_SIZE;
	#else
	i = 0;
	#endif
	for(;i < ARRAY_VECTOR_SIZE;i++){
		if(array_vector[i] == nullptr){
			break;
		}
	}
	return i;
}

array_t::array_t(void* tmp_pointer, std::string type, int_ settings__){
	add_int(std::make_pair(&id, "array_t: ID"));
	int_ entry = find_empty_array_entry();
	array_vector[entry] = this;
	id = scramble_id(entry);
	add_int(std::make_pair(&settings_, "array_t: settings"));
	set_settings(settings__);
	add_string(std::make_pair(&data_type, "array_t: data type"));
	data_type = type;
	add_string(std::make_pair(&name, "array_t: name"));
	// nothing to do here
	add_void_ptr(std::make_pair(&pointer, "array_t: pointer to object"));
	pointer = tmp_pointer;
	put_inside_type_vector = false;
	last_update = get_time();
	array_lock.lock();
	int_ type_entry = type_to_type_entry(data_type); // should return a new entry if the current type isn't on the list
	if(type_entry > -1){
		type_vector[type_entry].push_back(id);
	}
	array_lock.unlock();
}

void array_t::update_data(){
}

void array_t::reset_values(){
	data_lock.lock();
	int_lock.lock();
	const int_ int_array_size = int_array.size();
	for(int_ i = ARRAY_RESERVE_INT_SIZE;i < int_array_size;i++){ // first one is reserved for the array class
		*(std::get<0>(int_array[i])) = DEFAULT_INT_VALUE;
	}
	int_lock.unlock();
	string_lock.lock();
	const int_ string_array_size = string_array.size();
	for(int_ i = ARRAY_RESERVE_STRING_SIZE;i < string_array_size;i++){ // first one is reserved for the array class
		*(std::get<0>(string_array[i])) = DEFAULT_STRING_VALUE;
	}
	string_lock.unlock();
	long_double_lock.lock();
	const int_ long_double_array_size = long_double_array.size();
	for(int_ i = ARRAY_RESERVE_LONG_DOUBLE_SIZE;i < long_double_array_size;i++){
		*(std::get<0>(long_double_array[i])) = DEFAULT_LONG_DOUBLE_VALUE;
	}
	long_double_lock.unlock();
	void_ptr_lock.lock();
	const int_ void_ptr_array_size = void_ptr_array.size();
	for(int_ i = ARRAY_RESERVE_VOID_PTR_SIZE;i < void_ptr_array_size;i++){
		*(std::get<0>(void_ptr_array[i])) = nullptr;
	}
	void_ptr_lock.unlock();
	data_lock.unlock();
}

bool array_t::id_match(array_id_t a){
	return id == a;
}

void array_t::new_id(array_id_t new_id_){
	data_lock.lock();
	array_vector[strip_id(new_id_)] = this;
	if(id != 0){ // 0 = wasn't added to the array at all
		array_vector[strip_id(id)] = nullptr;
	}
	printf_(gen_print_prefix() + "Changed the ID from '" + std::to_string(id) + "' to '" + std::to_string(new_id_) + "'\n", PRINTF_DEBUG);
	id = new_id_;
	data_lock.unlock();
}

void array_t::parse_string_entry(std::string tmp_string){
	const array_id_t old_id = id;
	data_lock.lock();
	#ifdef CLASS_DEBUG_OUTPUT
	printf("tmp_string FROM parse_string_entry: %s\n", tmp_string.c_str());
	#endif
	last_update = get_time();
	if(tmp_string.find_first_of(ARRAY_INT_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing int_ from string\n");
		#endif
		parse_int_from_string(tmp_string);
	}
	if(tmp_string.find_first_of(ARRAY_LONG_DOUBLE_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing long_double from string\n");
		#endif
		parse_long_double_from_string(tmp_string);
	}
	if(tmp_string.find_first_of(ARRAY_STRING_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing string from string\n");
		#endif
		parse_string_from_string(tmp_string);
	}
	data_lock.unlock();
	if(old_id != id){
		array_id_t new_id_ = id;
		id = old_id;
		// the NEW ID needs to be passed as the parameter
		new_id(new_id_);
	}
}

std::string array_t::gen_updated_string(int_ what_to_update){
	data_lock.lock();
	std::string return_value = wrap(ARRAY_TYPE_SEPERATOR_START, data_type, ARRAY_TYPE_SEPERATOR_END) + wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END);
	SET_BIT(what_to_update, ARRAY_STRING_HASH_BIT, 1);
	SET_BIT(what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 1);
	SET_BIT(what_to_update, ARRAY_INT_HASH_BIT, 1);
	if(CHECK_BIT(what_to_update, ARRAY_STRING_HASH_BIT) == 1){
		return_value += gen_string_string();
	}
	if(CHECK_BIT(what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT) == 1){
		return_value += gen_long_double_string();
	}
	if(CHECK_BIT(what_to_update, ARRAY_INT_HASH_BIT) == 1){
		return_value += gen_int_string();
	}
	data_lock.unlock();
	return return_value;
}

uint_ array_t::pull_starting_number(std::string a){
	const int_ start = a.find_first_of(ARRAY_STARTING_START);
	const int_ end = a.find_first_of(ARRAY_STARTING_END);
	if(start == end){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("pull_starting_number start==end %s\n", a.c_str());
		#endif
	}
	return std::atoll(a.substr(start+1, end-start-1).c_str());
}

std::vector<std::string> pull_items_data(char *a, std::string b, char *c){
	std::vector<std::string> return_value;
	while(b.find_first_of(a) != std::string::npos || b.find_first_of(c) != std::string::npos){
		const int_ start = b.find_first_of(a);
		const int_ end = b.find_first_of(c);
		return_value.push_back(b.substr(start+1, end-start-1));
		b = b.substr(end+1, b.size());
	}
	return return_value;
}

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y, std::vector<int> *entries_for_data){
	assert(a.find_first_of(x) != a.find_first_of(y));
	std::vector<std::string> b;
	uint_ start;
	while((start = a.find_first_of(x)) != std::string::npos){
		const int_ end = a.find_first_of(y);
		std::string tmp_string_whole = a.substr(start+1, end-start-1);
		const int_ start_number = pull_starting_number(tmp_string_whole);
		#ifdef CLASS_DEBUG_OUTPUT
		printf("tmp_string_whole: %s\n", tmp_string_whole.c_str());
		#endif
		const std::vector<std::string> tmp_vector = pull_items_data(ARRAY_ITEM_SEPERATOR_START, tmp_string_whole, ARRAY_ITEM_SEPERATOR_END);
		for(uint_ i = 0;i < tmp_vector.size();i++){
			entries_for_data->push_back((int_)i+(int_)start_number);
		}
		b.insert(b.end(), tmp_vector.begin(), tmp_vector.end());
		a = a.substr(a.find_first_of(y)+1, a.size());
	}
	return b;
}

void array_t::parse_int_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> int_data = pull_items(ARRAY_INT_SEPERATOR_START, a, ARRAY_INT_SEPERATOR_END, &entries_for_data);
	const uint_ int_data_size = int_data.size();
	while(int_array.size() < int_data.size()){
		add_int(std::make_pair(new int_, "mysterious new variable"));
	}
	int_lock.lock();
	for(uint_ i = 0;i < int_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("int_data[%d]: %s\n", entries_for_data[i], int_data[i].c_str());
		#endif
		*(std::get<0>(int_array[entries_for_data[i]])) = std::atoll(int_data[i].c_str());
	}
	int_lock.unlock();
}

void array_t::parse_long_double_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> long_double_data = pull_items(ARRAY_LONG_DOUBLE_SEPERATOR_START, a, ARRAY_LONG_DOUBLE_SEPERATOR_END, &entries_for_data);
	const uint_ long_double_data_size = long_double_data.size();
	long_double_lock.lock();
	while(long_double_array.size() < long_double_data_size){
		long_double_array.push_back(std::make_pair(new long double, "mysterious new variable"));
	}
	for(uint_ i = 0;i < long_double_data_size;i++){
		*(std::get<0>(long_double_array[entries_for_data[i]])) = strtold(long_double_data[i].c_str(), nullptr);
	}
	long_double_lock.unlock();
}

void array_t::parse_string_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> string_data = pull_items(ARRAY_STRING_SEPERATOR_START, a, ARRAY_STRING_SEPERATOR_END, &entries_for_data);
	const uint_ string_data_size = string_data.size();
	string_lock.lock();
	while(string_array.size() < string_data_size){
		add_string(std::make_pair(new std::string, "mysterious new variable"));
	}
	for(uint_ i = 0;i < string_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("string_array[%d]: %s\n",entries_for_data[i], string_data[i].c_str());
		#endif
		*(std::get<0>(string_array[entries_for_data[i]])) = string_data[i];
	}
	string_lock.unlock();
}

static std::string array_gen_data_vector_entry(std::string data, int_ start){
	return wrap(ARRAY_STARTING_START, std::to_string(start), ARRAY_STARTING_END) + wrap(ARRAY_ITEM_SEPERATOR_START, data, ARRAY_ITEM_SEPERATOR_END);
}

bool array_t::updated( int_ *what_to_update){
	data_lock.lock();
	assert(what_to_update != nullptr);
	std::hash<std::string> hash_function;
	int_ tmp_string_hash = hash_function(gen_string_string());
	int_ tmp_long_double_hash = hash_function(gen_long_double_string());
	int_ tmp_int_hash = hash_function(gen_int_string());
	bool update = false;
	/*
	Chunking this up into smaller groups would make bigger arrays run faster.
	*/
	if(tmp_string_hash != string_hash){
		update = true;
		SET_BIT(*what_to_update, ARRAY_STRING_HASH_BIT, 1);
		string_hash = tmp_string_hash;
	}
	if(tmp_long_double_hash != long_double_hash){
		update = true;
		SET_BIT(*what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 1);
		long_double_hash = tmp_long_double_hash;
	}
	if(tmp_int_hash != int_hash){
		update = true;
		SET_BIT(*what_to_update, ARRAY_INT_HASH_BIT, 1);
		int_hash = tmp_int_hash;
	}
	data_lock.unlock();
	return update;
}

std::string array_t::gen_string_string(){
	std::string return_value;
	const int_ string_size = string_array.size();
	for(int_ i = 0;i < string_size;i++){
		std::string value = get_string(i);
		return_value += array_gen_data_vector_entry(value, i);
	}
	return_value = wrap(ARRAY_STRING_SEPERATOR_START, return_value, ARRAY_STRING_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_int_string(){
	std::string return_value;
	const int_ int_size = int_array.size();
	for(int_ i = 0;i < int_size;i++){
		int_ value = get_int(i);
		return_value += array_gen_data_vector_entry(std::to_string(value), i);
	}
	return_value = wrap(ARRAY_INT_SEPERATOR_START, return_value, ARRAY_INT_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_long_double_string(){
	std::string return_value;
	const int_ long_double_size = long_double_array.size();
	for(int_ i = 0;i < long_double_size;i++){
		long double value = get_long_double(i);
		return_value += array_gen_data_vector_entry(std::to_string(value), i);
	}
	return_value = wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, return_value, ARRAY_LONG_DOUBLE_SEPERATOR_END);
	return return_value;
}

std::vector<std::string> generate_outbound_class_data(){
	std::vector<std::string> return_value;
	for(uint_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		if(array_vector[i] != nullptr && array_vector[i]->get_send()){
			array_vector[i]->data_lock.lock();
			const std::string tmp = array_vector[i]->gen_updated_string(~0);
			array_vector[i]->data_lock.unlock();
			return_value.push_back(tmp);
		}
	}
	return return_value;
}

void update_class_data(std::string a, int_ what_to_update){
	array_id_t id = pull_id(a);
	array_t *tmp = (array_t*)find_array_pointer(id);
	if(tmp == nullptr){
		std::string type = a.substr(a.find_first_of(ARRAY_TYPE_SEPERATOR_START)+1, a.find_first_of(ARRAY_TYPE_SEPERATOR_END)-a.find_first_of(ARRAY_TYPE_SEPERATOR_START)-1);
		if(type == "coord_t"){
			tmp = &((new coord_t)->array);
		}else if(type == "model_t"){
			tmp = &((new model_t)->array);
		}else if(type == "client_t"){
			tmp = &((new client_t)->array);
		}else if(type == "net_ip_connection_info_t"){
			tmp = &((new net_ip_connection_info_t)->array);
		}else{
			printf("TODO: make a special allocator for '%s'\n", type.c_str());
			tmp = new array_t(nullptr, "", true);
		}
	}
	if(tmp->get_write_protected() == false){
		tmp->parse_string_entry(a);
	}
}

void add_two_arrays(array_t *a, array_t *b){
	a->data_lock.lock();
	b->data_lock.lock();
	a->int_array.insert(a->int_array.end(), b->int_array.begin(), b->int_array.end());
	a->long_double_array.insert(a->long_double_array.end(), b->long_double_array.begin(), b->long_double_array.end());
	a->string_array.insert(a->string_array.end(), b->string_array.begin(), b->string_array.end());
	a->data_lock.unlock();
	b->data_lock.unlock();
}

array_t::~array_t(){
	//printf_(print() + "DELETING AN ARRAY\n", PRINTF_STATUS);
	array_lock.lock();
	int_ array_pos_in_vector = strip_id(id);
	//assert(array_vector[array_pos_in_vector] == this);
	printf_(print(), PRINTF_DEBUG);
	array_vector[array_pos_in_vector] = nullptr;
	array_lock.unlock();
}

bool lower_id(array_t* a, array_t* y){
	return likely(a->id < y->id);
}

static array_id_t match_id_;

bool match_id(array_t* a){
	return match_id_ == a->id;
}

void *find_pointer(array_id_t id, std::string type){
	const array_id_t truncated_id = strip_id(id);
	if(array_vector[truncated_id] != nullptr && array_vector[truncated_id]->id == id){
		return array_vector[truncated_id]->pointer;
	}
	return nullptr;
}

array_t* find_array_pointer(array_id_t id){
	array_t *ptr = array_vector[strip_id(id)];
	//if(ptr == nullptr){
		//throw std::runtime_error(" ID is invalid");
	//}
	return ptr;
}

/*void delete_array_and_pointer(array_t *array){
	if(array == nullptr){
		return;
	}
	array->data_lock.lock();
	void *ptr = array->pointer;
	if(ptr == nullptr){
		delete array;
		return;
	}
	const std::string type = array->data_type;
	if(type == "coord_t"){
		delete (coord_t*)ptr;
	}else if(type == "model_t"){
		delete (model_t*)ptr;
	}else if(type == "client_t"){
		delete (client_t*)ptr;
	}else if(type == "net_ip_connection_info_t"){
		delete (net_ip_connection_info_t*)ptr;
	}else if(type == "render_t"){
		delete (render_t*)ptr;
	}else if(type == "input_t"){
		delete (input_t*)ptr;
	}else if(type == "net_t"){
		delete (net_t*)ptr;
	}else if(type == "image_t"){
		delete (image_t*)ptr;
	}else if(type == "tile_t"){
		delete (tile_t*)ptr;
	}else if(type == "loop_t"){
		//delete (loop_t*)ptr; // this is the only item that isn't dynamically allocated
	}else if(type == "screen_t"){
		delete (screen_t*)ptr;
	}else{
		printf_("delete_array_and_pointer: cannot delete the unknown data type '" + type + "'\n", PRINTF_ERROR);
	}
}

void delete_all_data(){
	for(int_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		delete_array_and_pointer(array_vector[i]);
		array_vector[i] = nullptr;
	}
}*/

std::string array_t::gen_print_prefix(){
	std::stringstream ss;
	ss << this;
	return "array_t::array_t (this = " + ss.str() + "): ";
}

static void* get_entry_pointer(std::pair<void*, std::string> a){
	return std::get<0>(a);
}

std::string array_t::print(){
	std::string output;
	std::stringstream ss_;
	ss_ << this;
	output += "Pointer: " + ss_.str() + "\n";
	int_lock.lock();
	const int_ int_array_size = int_array.size();
	for(int_ i = 0;i < int_array_size;i++){
		int* ptr = (int*)get_entry_pointer(int_array[i]);
		std::stringstream ptr_;
		ptr_ << ptr;
		output += "\tint_array[" + std::to_string(i) + "] (" + ptr_.str() + ") : " + std::to_string(*ptr) + "\t\t" + std::get<1>(int_array[i]) + "\n";
	}
	int_lock.unlock();
	string_lock.lock();
	const int_ max_print_string_length = 8192;
	const int_ string_array_size = string_array.size();
	for(int_ i = 0;i < string_array_size;i++){
		std::string *ptr = (std::string*)get_entry_pointer(string_array[i]);
		std::stringstream ptr_;
		ptr_ << ptr;
		if(std::get<0>(string_array[i])->size() < max_print_string_length){
			output += "\tstring_array[" + std::to_string(i) + "] (" + ptr_.str() + ") : " + *ptr + "\t" + std::get<1>(string_array[i]) + "\n";
		}else{
			output += "\tstring_array[" + std::to_string(i) + "] (" + ptr_.str() + ") is too large to print\n";
		}
	}
	string_lock.unlock();
	long_double_lock.lock();
	const int_ long_double_array_size = long_double_array.size();
	for(int_ i = 0;i < long_double_array_size;i++){
		long double* ptr = (long double*)get_entry_pointer(long_double_array[i]);
		std::stringstream ptr_;
		ptr_ << ptr;
		output += "\tlong_double_array[" + std::to_string(i) + "] (" + ptr_.str() + "): " + std::to_string(*ptr) + "\t" + std::get<1>(long_double_array[i]) + "\n";
	}
	const int_ void_ptr_array_size = void_ptr_array.size();
	for(int_ i = 0;i < void_ptr_array_size;i++){
		void* ptr = get_entry_pointer(void_ptr_array[i]);
		char pointer_to_pointer[512];
		int retval_to_ptr = snprintf(pointer_to_pointer, 512, "%p", ptr);
		char pointer_to_data[512];
		int retval_to_data = snprintf(pointer_to_data, 512, "%p", *(void**)ptr);
		output += "\tvoid_ptr_array[" + std::to_string(i) + "]: (" + std::string(pointer_to_pointer) + "): " + std::string(pointer_to_data) + "\t" + std::get<1>(void_ptr_array[i]) + "\n";
	}
	long_double_lock.unlock();
	output += "\n";
	std::stringstream ss;
	ss << &data_lock;
	output += "\tdata_lock address: " + ss.str() + "\n";
	return output;
}

std::vector<array_id_t> all_ids_of_type(std::string type){
	int_ entry = type_to_type_entry(type);
	if(entry > -1 && type_vector[entry].size() > 0){
		return type_vector[entry];
	}else{
		return std::vector<array_id_t>({0});
	}
}

bool valid_id(array_id_t a){
	return valid_int(a) && a != 0 && a != -1;
}

bool array_t::unlocked(){
	bool return_value = data_lock.try_lock();
	if(return_value == true){ // it has locked
		data_lock.unlock();
	}
	return return_value;
}

