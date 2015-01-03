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

long int array_scan_for_id(int); // IDs cannot be sent over the internet if they are any type other than long double, string, or int

extern std::vector<std::string> pull_items_data(char*, std::string, char*);

std::vector<array_t*> array_vector;

long int array_scan_for_id(long int id){
	const unsigned int array_vector_size = array_vector.size();
	for(unsigned int i = 0;i < array_vector_size;i++){
		if(unlikely(array_vector[i]->id == id)){
			return i;
		}
	}
	return -1;
}

long int array_highest_id(){
	long int id = 0;
	const unsigned int array_vector_size = array_vector.size();
	for(unsigned int i = 0;i < array_vector_size;i++){
		if(likely(array_vector[i]->id > id)){
			id = array_vector[i]->id;
		}
	}
	return id;
}

array_t::array_t(){
	id = gen_rand();
	if(id == -1){
		id = gen_rand();
	}
	int_array.push_back(&id);
}

bool array_t::id_match(int a){
	return id == a;
}

void array_t::parse_string_entry(std::string tmp_string){
	last_update = get_time();
	if(tmp_string.find_first_of(ARRAY_INT_SEPERATOR_START) != std::string::npos){
		parse_int_from_string(tmp_string);
	}
	if(tmp_string.find_first_of(ARRAY_LONG_DOUBLE_SEPERATOR_START) != std::string::npos){
		parse_long_double_from_string(tmp_string);
	}
	if(tmp_string.find_first_of(ARRAY_STRING_SEPERATOR_START) != std::string::npos){
		parse_string_from_string(tmp_string);
	}
}

void array_t::parse_string_vector(std::vector<std::vector<std::string>> a){
	const unsigned int a_size = a.size();
	for(unsigned int i = 0;i < a_size;i++){
		const unsigned int a_i_size = a[i].size();
		for(unsigned int n = 0;n < a_i_size;n++){
			parse_string_entry(a[i][n]);
		}
	}
}

unsigned int array_t::pull_starting_number(std::string a){
	unsigned long int start,end;
	start = a.find_first_of(ARRAY_STARTING_START);
	end = a.find_first_of(ARRAY_STARTING_END);
	if(start == end || start == std::string::npos){
		printf("An impossible error has taken place. There is no starting number for anything here! TAKE COVER\n");
		assert(false);
	}
	std::string return_value;
	return_value = a.substr(start+1, end-start-1);
	return atoi(return_value.c_str());
}

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y){
	std::vector<std::string> b;
	if(a.find_first_of(x) == std::string::npos){
		#ifdef CLASS_DEBUG
		printf("The string '%s' does not contain any instances of '%s'\n",a.c_str(),x);
		#endif
		return b;
	}
	unsigned long int seperator_end = a.find_first_of(y);
	unsigned long int seperator_start = a.find_first_of(x);
	if(seperator_start == std::string::npos){
		assert(false);
	}
	if(seperator_end == std::string::npos){
		seperator_end = a.size();
	}
	a = a.substr(a.find_first_of(x), a.find_first_of(y)-a.find_first_of(x));
	b = pull_items_data(ARRAY_ITEM_SEPERATOR_START, a, ARRAY_ITEM_SEPERATOR_END);
	return b;
}

void array_t::parse_int_from_string(std::string a){
	const unsigned int starting_point = pull_starting_number(a);
	std::vector<std::string> int_data = pull_items(ARRAY_INT_SEPERATOR_START,a,ARRAY_INT_SEPERATOR_END);
	const unsigned int int_data_size = int_data.size();
	for(unsigned int i = 0;i < int_data_size;i++){
		*int_array[starting_point+i] = atoi(int_data[i].c_str());
	}
}

void array_t::parse_long_double_from_string(std::string a){
	const unsigned long int starting_point = pull_starting_number(a);
	std::vector<std::string> long_double_data = pull_items(ARRAY_LONG_DOUBLE_SEPERATOR_START,a,ARRAY_LONG_DOUBLE_SEPERATOR_END);
	const unsigned long int long_double_data_size = long_double_data.size();
	for(unsigned long int i = 0;i < long_double_data_size;i++){
		*long_double_array[starting_point+i] = atof(long_double_data[i].c_str()); // long double version?
	}
}

void array_t::parse_string_from_string(std::string a){
	const unsigned long int starting_point = pull_starting_number(a);
	std::vector<std::string> string_data = pull_items(ARRAY_STRING_SEPERATOR_START, a, ARRAY_STRING_SEPERATOR_END);
	const unsigned long int string_data_size = string_data.size();
	for(unsigned long int i = 0;i < string_data_size;i++){
		*string_array[starting_point+i] = string_data[i];
	}
}

std::vector<std::string> array_t::gen_int_array_vector(){
	std::vector<std::string> return_value;
	const unsigned int int_array_size = int_array.size();
	for(unsigned int i = 0;i < int_array_size;i++){
		return_value.push_back(wrap(ARRAY_TYPE_SEPERATOR_START, data_type, ARRAY_TYPE_SEPERATOR_END) + wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(i), ARRAY_STARTING_END) + wrap(ARRAY_INT_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,std::to_string(*int_array[i]),ARRAY_ITEM_SEPERATOR_END), ARRAY_INT_SEPERATOR_END));
	}
	return return_value;
}

std::vector<std::string> array_t::gen_long_double_array_vector(){
	std::vector<std::string> return_value;
	const unsigned int long_double_array_size = long_double_array.size();
	for(unsigned int i = 0;i < long_double_array_size;i++){
		return_value.push_back(wrap(ARRAY_TYPE_SEPERATOR_START, data_type, ARRAY_TYPE_SEPERATOR_END) + wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(i), ARRAY_STARTING_END) + wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,std::to_string(*long_double_array[i]),ARRAY_ITEM_SEPERATOR_END), ARRAY_LONG_DOUBLE_SEPERATOR_END));
	}
	return return_value;
}

std::vector<std::string> array_t::gen_string_array_vector(){
	std::vector<std::string> return_value;
	const unsigned int string_array_size = string_array.size();
	for(unsigned int i = 0;i < string_array_size;i++){
		return_value.push_back(wrap(ARRAY_TYPE_SEPERATOR_START, data_type, ARRAY_TYPE_SEPERATOR_END) + wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(i), ARRAY_STARTING_END) + wrap(ARRAY_STRING_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START, *string_array[i], ARRAY_ITEM_SEPERATOR_END), ARRAY_STRING_SEPERATOR_END));
	}
	return return_value;
}

std::vector<std::vector<std::string>> array_t::gen_string_vector(){
	std::vector<std::vector<std::string>> return_value;
	return_value.push_back(gen_int_array_vector());
	return_value.push_back(gen_long_double_array_vector());
	return_value.push_back(gen_string_array_vector());
	return return_value;
}

void array_t::close(){
	int_array.clear();
	string_array.clear();
	long_double_array.clear();
}

void add_array_to_vector(array_t *a){
	array_vector.push_back(a);
}

void delete_array_from_vector(array_t *b){
	const unsigned int array_size = array_vector.size();
	for(unsigned int i = 0;i < array_size;i++){
		if(array_vector[i] == b){
			array_vector.erase(array_vector.begin()+i);
			break;
		}
	}
}

int pull_id(std::string a){
	int return_value;
	return_value = atoi(a.substr(a.find_first_of(ARRAY_ID_START), a.find_first_of(ARRAY_ID_END)).c_str());
	return return_value;
}

void update_class_data(std::string a, int what_to_update){
	array_t *tmp = nullptr;
	int id = pull_id(a);
	const unsigned long int array_vector_size = array_vector.size();
	for(unsigned long int i = 0;i < array_vector_size;i++){
		if(unlikely(array_vector[i]->id == id)){
			tmp = array_vector[i];
			return;
		}
	}
	if(tmp == nullptr){
		std::string type = a.substr(a.find_first_of(ARRAY_TYPE_SEPERATOR_START), a.find_first_of(ARRAY_TYPE_SEPERATOR_END)-a.find_first_of(ARRAY_TYPE_SEPERATOR_START));
		if(type == "coord_t"){
			if((what_to_update&(1<<0)) == 1){
				coord_vector.push_back(new coord_t);
				tmp = &coord_vector[coord_vector.size()-1]->array;
			}
		}else if(type == "model_t"){
			if((what_to_update&(1<<1)) == 1){
				model_vector.push_back(new model_t);
				tmp = &model_vector[model_vector.size()-1]->array;
			}
		}else if(type == "input_buffer_t"){
			if((what_to_update&(1<<2)) == 1){
				input_buffer_vector.push_back(new input_buffer_t);
				tmp = &input_buffer_vector[input_buffer_vector.size()-1]->array;
			}
		}else if(type == "render_buffer_t"){
			if((what_to_update&(1<<3)) == 1){
				render_buffer_vector.push_back(new render_buffer_t);
				tmp = &render_buffer_vector[render_buffer_vector.size()-1]->array;
			}
		}else if(type == "client_t"){
			if((what_to_update&(1<<4)) == 1){
				client_vector.push_back(new client_t);
				tmp = &client_vector[client_vector.size()-1]->array;
			}
		}else{
			assert(false);
		}
	}
	if(tmp != nullptr){
		tmp->parse_string_entry(a);
	}
}

void add_two_arrays(array_t *a, array_t *b){
	a->int_array.insert(a->int_array.end(), b->int_array.begin(), b->int_array.end());
	a->long_double_array.insert(a->long_double_array.end(), b->long_double_array.begin(), b->long_double_array.end());
	a->string_array.insert(a->string_array.end(), b->string_array.begin(), b->string_array.end());
}

std::string array_t::generate_string_from_variable(long double* a){
	unsigned int vector_starting_point = 0;
	const unsigned int long_double_size = long_double_array.size();
	for(unsigned int i = 0;i < long_double_size;i++){
		if(long_double_array[i] == a){
			vector_starting_point = i;
			break;
		}
	}
	return (wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(vector_starting_point), ARRAY_STARTING_END) + wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,std::to_string(*a),ARRAY_ITEM_SEPERATOR_END), ARRAY_LONG_DOUBLE_SEPERATOR_END));
}

std::string array_t::generate_string_from_variable(std::string* a){
	unsigned int vector_starting_point = 0;
	const unsigned int std_string_size = long_double_array.size();
	for(unsigned int i = 0;i < std_string_size;i++){
		if(string_array[i] == a){
			vector_starting_point = i;
			break;
		}
	}
	return (wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(vector_starting_point), ARRAY_STARTING_END) + wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,*a,ARRAY_ITEM_SEPERATOR_END), ARRAY_LONG_DOUBLE_SEPERATOR_END));
}

std::string array_t::generate_string_from_variable(int* a){
	unsigned int vector_starting_point = 0;
	const unsigned int int_size = long_double_array.size();
	for(unsigned int i = 0;i < int_size;i++){
		if(int_array[i] == a){
			vector_starting_point = i;
			break;
		}
	}
	return (wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(vector_starting_point), ARRAY_STARTING_END) + wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,std::to_string(*a),ARRAY_ITEM_SEPERATOR_END), ARRAY_LONG_DOUBLE_SEPERATOR_END));
}

void delete_array_id(int id){
	const unsigned long int array_size = array_vector.size();
	for(unsigned long int i = 0;i < array_size;i++){
		if(array_vector[i]->id == id){
			delete array_vector[i];
			array_vector.erase(array_vector.begin()+i);
			return;
		}
	}
}

bool array_t::updated(int *what_to_update){
	assert(what_to_update != nullptr);
	std::hash<std::string> hash_function;
	long int tmp_string_hash = hash_function(gen_string_string());
	long int tmp_long_double_hash = hash_function(gen_long_double_string());
	long int tmp_int_hash = hash_function(gen_int_string());
	bool update = false;
	if(tmp_string_hash != string_hash){
		update = true;
		(*what_to_update) |= 1 << ARRAY_STRING_HASH_BIT;
		string_hash = tmp_string_hash;
	}
	if(tmp_long_double_hash != long_double_hash){
		update = true;
		(*what_to_update) |= 1 << ARRAY_LONG_DOUBLE_HASH_BIT;
		long_double_hash = tmp_long_double_hash;
	}
	if(tmp_int_hash != int_hash){
		update = true;
		(*what_to_update) |= 1 << ARRAY_INT_HASH_BIT;
		int_hash = tmp_int_hash;
	}
	return update;
}

std::string array_t::gen_string(){
	std::vector<std::vector<std::string>> tmp = gen_string_vector();
	std::string return_value;
	for(unsigned long int i = 0;i < tmp.size();i++){
		for(unsigned long int n = 0;n < tmp[i].size();n++){
			return_value += tmp[i][n];
		}
	}
	return return_value;
}

std::string array_t::gen_string_string(){
	std::vector<std::string> tmp = gen_string_array_vector();
	std::string return_value;
	for(unsigned long int i = 0;i < tmp.size();i++){
		return_value += tmp[i];
	}
	return return_value;
}

std::string array_t::gen_int_string(){
	std::vector<std::string> tmp = gen_int_array_vector();
	std::string return_value;
	for(unsigned long int i = 0;i < tmp.size();i++){
		return_value += tmp[i];
	}
	return return_value;
}

std::string array_t::gen_long_double_string(){
	std::vector<std::string> tmp = gen_long_double_array_vector();
	std::string return_value;
	for(unsigned long int i = 0;i < tmp.size();i++){
		return_value += tmp[i];
	}
	return return_value;
}
