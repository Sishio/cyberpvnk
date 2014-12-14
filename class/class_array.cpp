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
#include "class_array.h"

static std::vector<array_t*> array_vector;

static std::string wrap(char *start, std::string data, char *end){
	return (std::string)start + data + (std::string)end;
}

array_t::array_t(){
	id = gen_rand();
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

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y){
	std::vector<std::string> b;
	if(a.find_first_of(x) == std::string::npos){
		#ifdef CLASS_DEBUG
		printf("The string '%s' does not contain any instances of '%s'\n",a.c_str(),x);
		#endif
		return b;
	}
	#ifdef CLASS_DEBUG
	printf("The string that is about to be processed has '%s' after the initial slice\n",a.c_str());
	#endif
	while(true){
		const size_t start_start = a.find_first_of(ARRAY_ITEM_SEPERATOR_START);
		const size_t end_start = a.find_first_of(ARRAY_ITEM_SEPERATOR_END);
		const size_t start_end = start_start + strlen(ARRAY_ITEM_SEPERATOR_START);
		const size_t end_end = end_start + strlen(ARRAY_ITEM_SEPERATOR_END);
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
			printf("start_start:%lu\tend_start:%lu\tstart_end:%lu\tend_end:%lu\n",start_start, end_start, start_end, end_end);
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
	printf("The starting number from the string '%s' is %lu\n",a.c_str(), starting_point);
	std::vector<std::string> long_double_data = pull_items(ARRAY_LONG_DOUBLE_SEPERATOR_START,a,ARRAY_LONG_DOUBLE_SEPERATOR_END);
	const unsigned long int long_double_data_size = long_double_data.size();
	for(unsigned long int i = 0;i < long_double_data_size;i++){
		const long double z = atof(long_double_data[i].c_str());
		#ifdef CLASS_DEBUG
		printf("The version of the double that was received and will be used:%Lf\n",z);
		#endif
		*long_double_array[starting_point+i] = z;
	}
	for(unsigned long int i = 0;i < long_double_data_size;i++){
		printf("long_double_array[%lu]:%Lf\n", i, *long_double_array[i]);
	}
}

void array_t::parse_string_from_string(std::string a){
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

void update_data(std::string a){
	int id = pull_id(a);
	const int array_vector_size = array_vector.size();
	for(int i = 0;i < array_vector_size;i++){
		if(array_vector[i]->id == id){
			array_vector[i]->parse_string_entry(a);
			break;
		}
	}
}
