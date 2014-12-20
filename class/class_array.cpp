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

static std::vector<array_t*> array_vector;

long int array_scan_for_id(int id){
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

static std::string wrap(char *start, std::string data, char *end){
	return (std::string)start + data + (std::string)end;
}

array_t::array_t(){
	hash = 0;
	id = gen_rand();
	if(id == -1){
		id = gen_rand();
	}
}

bool array_t::id_match(int a){
	return id == a;
}

void array_t::parse_string_entry(std::string a){
	parse_int_from_string(a);
	parse_long_double_from_string(a);
	parse_string_from_string(a);
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
		printf("An impossible error has taken place. TAKE COVER\n");
		assert(false);
	}
	std::string return_value;
	return_value = a.substr(start+1, end-start-1);
	return atoi(return_value.c_str());
}

std::vector<std::string> array_t::pull_items_data(char *c, std::string a, char *d){
	std::vector<std::string> b;
	while(true){
		const size_t start_start = a.find_first_of(c);
		const size_t end_start = a.find_first_of(d);
		const size_t start_end = start_start + strlen(c);
		const size_t end_end = end_start + strlen(d);
		bool conditional[8] = {false};
		conditional[4] = start_start == std::string::npos;
		conditional[5] = end_start == std::string::npos;
		if(conditional[4] || conditional[5]){ // escape condition
			#ifdef CLASS_DEBUG
			printf("An expected condition has been met\n");
			for(unsigned int i = 4;i < 6;i++){
				printf("%d\n",conditional[i]);
			}
			#endif
			break;
		}
		conditional[0] = start_start == end_start && start_start != std::string::npos;
		conditional[1] = start_end == end_end && start_start != std::string::npos;
		conditional[2] = start_start > end_start;
		conditional[3] = start_end > end_end;
		if(conditional[0] || conditional[1] || conditional[2] || conditional[3]){
			printf("An impossible condition has been met\n");
			for(unsigned int i = 0;i < 4;i++){
				printf("%d\n",conditional[i]);
			}
			printf("start_start:%zu\tend_start:%zu\tstart_end:%zu\tend_end:%zu\n",start_start, end_start, start_end, end_end);
			assert(false);
		}
		#ifdef CLASS_DEBUG
		printf("%lu %lu %lu %lu\n",start_start, start_end, end_start, end_end);
		#endif
		std::string z = a.substr(start_end,(end_start-start_end)-1);
		#ifdef CLASS_DEBUG
		printf("Parsed data:%s\n",z.c_str());
		#endif
		b.push_back(z);
		a = a.substr(end_end+1,a.size());
	}
	return b;
}

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y){
	std::vector<std::string> b;
	if(a.find_first_of(x) == std::string::npos){
		#ifdef CLASS_DEBUG
		printf("The string '%s' does not contain any instances of '%s'\n",a.c_str(),x);
		#endif
		return b;
	}
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
		return_value.push_back(wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(i), ARRAY_STARTING_END) + wrap(ARRAY_INT_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,std::to_string(*int_array[i]),ARRAY_ITEM_SEPERATOR_END), ARRAY_INT_SEPERATOR_END));
	}
	return return_value;
}

std::vector<std::string> array_t::gen_long_double_array_vector(){
	std::vector<std::string> return_value;
	const unsigned int long_double_array_size = long_double_array.size();
	for(unsigned int i = 0;i < long_double_array_size;i++){
		return_value.push_back(wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(i), ARRAY_STARTING_END) + wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START,std::to_string(*long_double_array[i]),ARRAY_ITEM_SEPERATOR_END), ARRAY_LONG_DOUBLE_SEPERATOR_END));
	}
	return return_value;
}

std::vector<std::string> array_t::gen_string_array_vector(){
	std::vector<std::string> return_value;
	const unsigned int string_array_size = string_array.size();
	for(unsigned int i = 0;i < string_array_size;i++){
		// this string is broken
		return_value.push_back(wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + wrap(ARRAY_STARTING_START, std::to_string(i), ARRAY_STARTING_END) + wrap(ARRAY_STRING_SEPERATOR_START, wrap(ARRAY_ITEM_SEPERATOR_START, *string_array[i], ARRAY_ITEM_SEPERATOR_END), ARRAY_STRING_SEPERATOR_END));
	}
	return return_value;
}

std::vector<std::vector<std::string>> array_t::gen_string_vector(bool force){
	int tmp_hash = gen_hash();
	if(force == false){
		force = tmp_hash == hash;
	};
	std::vector<std::vector<std::string>> return_value;
	if(force){
		hash = tmp_hash;
		return_value.push_back(gen_int_array_vector());
		return_value.push_back(gen_long_double_array_vector());
		return_value.push_back(gen_string_array_vector());
	}
	return return_value;
}

void array_t::close(){
	int_array.clear();
	string_array.clear();
	long_double_array.clear();
}

int array_t::gen_hash(){
	int new_hash = encrypt(int_array) + encrypt(long_double_array) + encrypt(string_array);
	return new_hash;
}

array_t *new_array(){
	array_t *a = new array_t;
	array_vector.push_back(a);
	return a;
}

void delete_array(array_t *b){
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

void update_class_data(std::string a){
	int id = pull_id(a);
	const int array_vector_size = array_vector.size();
	for(int i = 0;i < array_vector_size;i++){
		if(array_vector[i]->id == id){
			array_vector[i]->parse_string_entry(a);
			break;
		}
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
