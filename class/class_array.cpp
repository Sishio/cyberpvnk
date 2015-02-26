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
#include "class_main.h"
#include "class_array.h"

//#define CLASS_DEBUG_OUTPUT 1

std::vector<array_t*> array_vector;
std::mutex array_lock;

static array_id_t pull_id(std::string a){
	int return_value;
	const unsigned long int start = a.find_first_of(ARRAY_ID_START);
	const unsigned long int end = a.find_first_of(ARRAY_ID_END);
	std::string id_string = a.substr(start+1, end-start-1).c_str();
	return_value = atoi(id_string.c_str());
	return return_value;
}

array_id_t array_highest_id(){
	array_id_t id = 0;
	const unsigned long int array_vector_size = array_vector.size();
	for(unsigned long int i = 0;i < array_vector_size;i++){
		if(likely(array_vector[i]->id > id)){
			id = array_vector[i]->id;
		}
	}
	return id;
}

array_t::array_t(void* tmp_pointer, bool add_array_to_vector){
	pointer = tmp_pointer;
	id = (array_id_t)gen_rand();
	if(id == 0){
		id = (array_id_t)gen_rand();
	}
	update_pointers();
	if(add_array_to_vector){
		array_lock.lock();
		array_vector.push_back(this);
		array_lock.unlock();
	}
	last_update = get_time();
}

bool array_t::id_match(array_id_t a){
	return id == a;
}

void array_t::parse_string_entry(std::string tmp_string){
	#ifdef CLASS_DEBUG_OUTPUT
	printf("tmp_string FROM parse_string_entry: %s\n", tmp_string.c_str());
	#endif
	last_update = get_time();
	if(tmp_string.find_first_of(ARRAY_INT_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing int from string\n");
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
}

std::string array_t::gen_updated_string(int what_to_update){
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
	return return_value;
}

void array_t::update_pointers(){
	int_array.clear();
	int_array.push_back(&id);
}

unsigned int array_t::pull_starting_number(std::string a){
	const unsigned long int start = a.find_first_of(ARRAY_STARTING_START);
	const unsigned long int end = a.find_first_of(ARRAY_STARTING_END);
	if(start == end){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("pull_starting_number start==end %s\n", a.c_str());
		#endif
	}
	return atoi(a.substr(start+1, end-start-1).c_str());
}

std::vector<std::string> pull_items_data(char *a, std::string b, char *c){
	std::vector<std::string> return_value;
	while(b.find_first_of(a) != std::string::npos || b.find_first_of(c) != std::string::npos){
		const unsigned long int start = b.find_first_of(a);
		const unsigned long int end = b.find_first_of(c);
		return_value.push_back(b.substr(start+1, end-start-1));
		b = b.substr(end+1, b.size());
	}
	return return_value;
}

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y, std::vector<int> *entries_for_data){
	assert(a.find_first_of(x) != a.find_first_of(y));
	std::vector<std::string> b;
	unsigned long int start;
	while((start = a.find_first_of(x)) != std::string::npos){
		const unsigned long int end = a.find_first_of(y);
		std::string tmp_string_whole = a.substr(start+1, end-start-1);
		const unsigned long int start_number = pull_starting_number(tmp_string_whole);
		#ifdef CLASS_DEBUG_OUTPUT
		printf("tmp_string_whole: %s\n", tmp_string_whole.c_str());
		#endif
		const std::vector<std::string> tmp_vector = pull_items_data(ARRAY_ITEM_SEPERATOR_START, tmp_string_whole, ARRAY_ITEM_SEPERATOR_END);
		for(unsigned long int i = 0;i < tmp_vector.size();i++){
			entries_for_data->push_back((int)i+(int)start_number);
		}
		b.insert(b.end(), tmp_vector.begin(), tmp_vector.end());
		a = a.substr(a.find_first_of(y)+1, a.size());
	}
	return b;
}

void array_t::parse_int_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> int_data = pull_items(ARRAY_INT_SEPERATOR_START, a, ARRAY_INT_SEPERATOR_END, &entries_for_data);
	const unsigned long int int_data_size = int_data.size();
	int_lock.lock();
	for(unsigned long int i = 0;i < int_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("int_data[%d]: %s\n", entries_for_data[i], int_data[i].c_str());
		#endif
		*int_array[entries_for_data[i]] = atoi(int_data[i].c_str());
	}
	int_lock.unlock();
}

void array_t::parse_long_double_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> long_double_data = pull_items(ARRAY_LONG_DOUBLE_SEPERATOR_START, a, ARRAY_LONG_DOUBLE_SEPERATOR_END, &entries_for_data);
	const unsigned long int long_double_data_size = long_double_data.size();
	long_double_lock.lock();
	for(unsigned long int i = 0;i < long_double_data_size;i++){
          *long_double_array[entries_for_data[i]] = strtold(long_double_data[i].c_str(), nullptr);
	}
	long_double_lock.unlock();
}

void array_t::parse_string_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> string_data = pull_items(ARRAY_STRING_SEPERATOR_START, a, ARRAY_STRING_SEPERATOR_END, &entries_for_data);
	const unsigned long int string_data_size = string_data.size();
	string_lock.lock();
	for(unsigned long int i = 0;i < string_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("string_array[%d]: %s\n",entries_for_data[i], string_data[i].c_str());
		#endif
		*string_array[entries_for_data[i]] = string_data[i];
	}
	string_lock.unlock();
}

static std::string array_gen_data_vector_entry(std::string data, unsigned int start){
	return wrap(ARRAY_STARTING_START, std::to_string(start), ARRAY_STARTING_END) + wrap(ARRAY_ITEM_SEPERATOR_START, data, ARRAY_ITEM_SEPERATOR_END);
}

bool array_t::updated(int *what_to_update){
	assert(what_to_update != nullptr);
	std::hash<std::string> hash_function;
	long int tmp_string_hash = hash_function(gen_string_string());
	long int tmp_long_double_hash = hash_function(gen_long_double_string());
	long int tmp_int_hash = hash_function(gen_int_string());
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
	return update;
}

std::string array_t::gen_string_string(){
	std::string return_value;
	const unsigned long int string_size = string_array.size();
	for(unsigned long int i = 0;i < string_size;i++){
		return_value += array_gen_data_vector_entry((std::string)*string_array[i], (unsigned int)i);
	}
	return_value = wrap(ARRAY_STRING_SEPERATOR_START, return_value, ARRAY_STRING_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_int_string(){
	std::string return_value;
	const unsigned long int int_size = int_array.size();
	for(unsigned long int i = 0;i < int_size;i++){
		return_value += array_gen_data_vector_entry(std::to_string((int)*int_array[i]), (unsigned int)i);
	}
	return_value = wrap(ARRAY_INT_SEPERATOR_START, return_value, ARRAY_INT_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_long_double_string(){
	std::string return_value;
	const unsigned long int long_double_size = long_double_array.size();
	for(unsigned long int i = 0;i < long_double_size;i++){
		return_value += array_gen_data_vector_entry(std::to_string((long double)*long_double_array[i]), (unsigned int)i);
	}
	return_value = wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, return_value, ARRAY_LONG_DOUBLE_SEPERATOR_END);
	return return_value;
}

void update_class_data(std::string a, int what_to_update){
	array_t *tmp = nullptr;
	array_id_t id = pull_id(a);
	const unsigned long int array_vector_size = array_vector.size();
	for(unsigned long int i = 0;i < array_vector_size;i++){
		if(unlikely(array_vector[i]->id == id)){
			tmp = array_vector[i];
			break;
		}
	}
	if(tmp == nullptr){
		std::string type = a.substr(a.find_first_of(ARRAY_TYPE_SEPERATOR_START)+1, a.find_first_of(ARRAY_TYPE_SEPERATOR_END)-a.find_first_of(ARRAY_TYPE_SEPERATOR_START)-1);
		if(type == "coord_t"){
			tmp = &((new coord_t)->array);
		}else if(type == "model_t"){
			tmp = &((new model_t)->array);
		}else if(type == "input_buffer_t"){
			tmp = &((new input_buffer_t)->array);
		}else if(type == "client_t"){
			tmp = &((new client_t)->array);
		}else if(type == "net_ip_connection_info_t"){
			tmp = &((new net_ip_connection_info_t)->array);
		}else if(type == "gametype_t"){
			tmp = &((new gametype_t(""))->array);
		}else{
			assert(false);
		}
	}
	tmp->parse_string_entry(a);
}

void add_two_arrays(array_t *a, array_t *b){
	a->int_array.insert(a->int_array.end(), b->int_array.begin(), b->int_array.end());
	a->long_double_array.insert(a->long_double_array.end(), b->long_double_array.begin(), b->long_double_array.end());
	a->string_array.insert(a->string_array.end(), b->string_array.begin(), b->string_array.end());
}

array_t::~array_t(){
	std::vector<array_t*>::iterator array_pos_in_vector = std::find_if(array_vector.begin(), array_vector.end(), pointer_device_t(this));
	if(likely(array_pos_in_vector != array_vector.end())){
		array_lock.lock();
		array_vector.erase(array_pos_in_vector);
		array_lock.unlock();
	}else{
		printf("Could not find array in vector\n");
	}
}

static bool alphabetical_order(const array_t *a, const array_t *b){return a->data_type < b->data_type;}

static bool order_by_id(const array_t *a, const array_t *b){
	return a->id < b->id;
}

static bool array_is_sorted_by_type(){
	for(unsigned long int i = 0;i < array_vector.size()-1;i++){
		if(array_vector[i]->data_type > array_vector[i+1]->data_type){
			return false;
		}
	}
	return true;
}

static bool array_is_sorted_by_id(){
	for(unsigned long int i = 1;i < array_vector.size();i++){
		const std::string old_type = array_vector[i-1]->data_type;
		const array_id_t old_id = array_vector[i-1]->id;
		if(unlikely(old_id > array_vector[i]->id && old_type == array_vector[i]->data_type)){
			return false;
		}
	}
	return true;
}

static void optimal_array_sort(){
	array_lock.lock();
	if(unlikely(!array_is_sorted_by_type())){
		std::sort(array_vector.begin(), array_vector.end(), alphabetical_order); // sort by the value of the data_type
	}
	if(unlikely(!array_is_sorted_by_id())){
		const unsigned long int array_size = array_vector.size();
		for(unsigned long int i = 0;i < array_size;i++){ // sort all of the data_types (in groups) by their ID
			const std::string old_type = array_vector[i]->data_type;
			const unsigned long int old_entry = i;
			while(likely(old_type == array_vector[i]->data_type && i < array_size-1)){
				i++;
			}
			std::sort(array_vector.begin()+old_entry, array_vector.begin()+i, order_by_id);
		}
	}
	array_lock.unlock();
}

void *find_pointer(array_id_t id, std::string type){
	optimal_array_sort();
	void* return_value = nullptr;
	std::vector<void*> *array;
	if(type != ""){
		array = new std::vector<void*>;
		*array = all_entries_of_type(type);
	}else{
		array = (std::vector<void*>*)&array_vector;
		array_lock.lock();
	}
	const unsigned long int array_size = array_vector.size();
	for(unsigned long int i = 0;i < array_size;i++){
		if(unlikely(((array_t*)(*array)[i])->id == id)){
			return_value = ((array_t*)(*array)[i])->pointer;
			break;
		}
	}
	if(type != ""){
		delete array;
		array = nullptr;
	}else{
		array_lock.unlock();
	}
	return return_value;
}

std::vector<void*> all_entries_of_type(std::string type){
	std::vector<void*> return_value;
	for(unsigned long int i = 0;i < array_vector.size();i++){
		if(unlikely(array_vector[i]->data_type == type)){
			return_value.push_back((void*)array_vector[i]);
		}
	}
	return return_value;
}

std::vector<void*> all_pointers_of_type(std::string type){
	std::vector<void*> return_value;
	for(unsigned long int i = 0;i < array_vector.size();i++){
		if(unlikely(array_vector[i]->data_type == type)){
			return_value.push_back((void*)(((array_t*)array_vector[i])->pointer));
		}
	}
	return return_value;
}

array_t* find_array_pointer(int id){
	const unsigned long int array_vector_size = array_vector.size();
	for(unsigned long int i = 0;i < array_vector_size;i++){
		if(array_vector[i]->id == id){
			return array_vector[i];
		}
	}
	return nullptr;
}

void delete_array_and_pointer(array_t *array){
	void *ptr = array->pointer;
	const std::string type = array->data_type;
	if(type == "coord_t"){
		delete (coord_t*)ptr;
	}else if(type == "model_t"){
		delete (model_t*)ptr;
	}else if(type == "input_buffer_t"){
		delete (input_buffer_t*)ptr;
	}else if(type == "input_setings_t"){
		delete (input_settings_t*)ptr;
	}else if(type == "client_t"){
		delete (client_t*)ptr;
	}else if(type == "gametype_t"){
		delete (gametype_t*)ptr;
	}else if(type == "net_ip_connection_info_t"){
		delete (net_ip_connection_info_t*)ptr;
	}else{
		printf("cannot delete the unknown data type %s\n", type.c_str());
	}
}

void delete_all_data(){
	std::vector<array_t*> array_vector_ = array_vector; // make a copy of it, something breaks when I don't make a copy
	const unsigned long int array_vector__size = array_vector_.size();
	for(unsigned long int i = 0;i < array_vector__size;i++){
		delete_array_and_pointer(array_vector_[i]);
	}
}

std::string array_t::print(){
	std::string output;
	output += "Type: " + data_type + "\t";
	output += "ID: " + std::to_string(id) + "\n";
	int_lock.lock();
	const unsigned long int int_array_size = int_array.size();
	for(unsigned long int i = 0;i < int_array_size;i++){
		output += "\tint_array[" + std::to_string(i) + "]: " + std::to_string(*int_array[i]) + "\n";
	}
	int_lock.unlock();
	string_lock.lock();
	const unsigned long int max_print_string_length = 8192;
	const unsigned long int string_array_size = string_array.size();
	for(unsigned long int i = 0;i < string_array_size;i++){
		if(string_array[i]->size() < max_print_string_length){
			output += "\tstring_array[" + std::to_string(i) + "]: " + *string_array[i] + "\n";
		}else{
			output += "\tstring_array[" + std::to_string(i) + "] is too large to print\n";
		}
	}
	string_lock.unlock();
	long_double_lock.lock();
	const unsigned long int long_double_array_size = long_double_array.size();
	for(unsigned long int i = 0;i < long_double_array_size;i++){
		output += "\tlong_double_array[" + std::to_string(i) + "]: " + std::to_string(*long_double_array[i]) + "\n";
	}
	long_double_lock.unlock();
	output += "\n";
	printf("%s", output.c_str());
	return output;
}


