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

static unsigned int pull_starting_number(std::string a){
	return atoi(a.substr(a.find_first_of(ARRAY_STARTING_START), a.find_first_of(ARRAY_STARTING_END)).c_str());
}

void array_t::parse_int_from_string(std::string a){
	const unsigned int start = pull_starting_number(a);
	const unsigned int int_array_size = int_array.size();
	for(unsigned int i = start;i < int_array_size;i++){
		*int_array[i] = atoi(a.substr(a.find_first_of(ARRAY_ITEM_SEPERATOR_START)+1, a.find_first_of(ARRAY_ITEM_SEPERATOR_END)).c_str());
		a = a.substr(a.find_first_of(ARRAY_ITEM_SEPERATOR_END)+1, a.size());
	}
}

void array_t::parse_long_double_from_string(std::string a){
	const unsigned int start = pull_starting_number(a);
	const unsigned int long_double_array_size = long_double_array.size();
	for(unsigned int i = start;i < long_double_array_size;i++){
		*long_double_array[i] = atof(a.substr(a.find_first_of(ARRAY_ITEM_SEPERATOR_START)+1, a.find_first_of(ARRAY_ITEM_SEPERATOR_END)).c_str());
		a = a.substr(a.find_first_of(ARRAY_ITEM_SEPERATOR_END)+1, a.size());
	}
}

void array_t::parse_string_from_string(std::string a){
	const unsigned int start = pull_starting_number(a);
	const unsigned int string_array_size = string_array.size();
	for(unsigned int i = start;i < string_array_size;i++){
		*string_array[i] = a.substr(a.find_first_of(ARRAY_ITEM_SEPERATOR_START)+1, a.find_first_of(ARRAY_ITEM_SEPERATOR_END));
		a = a.substr(a.find_first_of(ARRAY_ITEM_SEPERATOR_END)+1, a.size());
	}
}

std::vector<std::string> array_t::gen_int_array_vector(){
	std::vector<std::string> return_value;
	std::string tmp_string;
	const unsigned int int_array_size = int_array.size();
	unsigned int starting_point = 0;
	tmp_string = wrap(ARRAY_STARTING_START, std::to_string(starting_point), ARRAY_STARTING_END);
	for(unsigned int i = 0;i < int_array_size;i++){
		tmp_string += wrap(ARRAY_ITEM_SEPERATOR_START, std::to_string(*int_array[i]), ARRAY_ITEM_SEPERATOR_END);
		printf("%s\n",tmp_string.c_str());
		if(tmp_string.size() > 484-10){
			return_value.push_back(tmp_string);
			tmp_string = wrap(ARRAY_STARTING_START, std::to_string(starting_point), ARRAY_STARTING_END);
		}
	}
	if(tmp_string != ""){
		return_value.push_back(tmp_string);
	}
	const unsigned int return_value_size = return_value.size();
	for(unsigned int i = 0;i < return_value_size;i++){
		return_value[i] = wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + return_value[i];
		printf("%s\n",return_value[i].c_str());
	}

	return return_value;
}

std::vector<std::string> array_t::gen_long_double_array_vector(){
	std::vector<std::string> return_value;
	std::string tmp_string;
	const unsigned int long_double_array_size = long_double_array.size();
	unsigned int starting_point = 0;
	tmp_string = wrap(ARRAY_STARTING_START, std::to_string(starting_point), ARRAY_STARTING_END);
	for(unsigned int i = 0;i < long_double_array_size;i++){
		tmp_string += wrap(ARRAY_ITEM_SEPERATOR_START, std::to_string(*long_double_array[i]), ARRAY_ITEM_SEPERATOR_END);
		if(tmp_string.size() > 484-10){
			return_value.push_back(tmp_string);
			tmp_string = wrap(ARRAY_STARTING_START, std::to_string(starting_point), ARRAY_STARTING_END);
		}
	}
	if(tmp_string != ""){
		return_value.push_back(tmp_string);
	}
	const unsigned int return_value_size = return_value.size();
	for(unsigned int i = 0;i < return_value_size;i++){
		return_value[i] = wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + return_value[i];
	}
	return return_value;
}

std::vector<std::string> array_t::gen_string_array_vector(){
	std::vector<std::string> return_value;
	std::string tmp_string;
	const unsigned int string_array_size = string_array.size();
	unsigned int starting_point = 0;
	tmp_string = wrap(ARRAY_STARTING_START, std::to_string(starting_point), ARRAY_STARTING_END);
	for(unsigned int i = 0;i < string_array_size;i++){
		tmp_string += wrap(ARRAY_ITEM_SEPERATOR_START, *string_array[i], ARRAY_ITEM_SEPERATOR_END);
		if(tmp_string.size() > 484-10){
			return_value.push_back(tmp_string);
			tmp_string = wrap(ARRAY_STARTING_START, std::to_string(starting_point), ARRAY_STARTING_END);
		}
	}
	if(tmp_string != ""){
		return_value.push_back(tmp_string);
	}
	const unsigned int return_value_size = return_value.size();
	for(unsigned int i = 0;i < return_value_size;i++){
		return_value[i] = wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END) + return_value[i];
	}
	return return_value;
}

std::vector<std::vector<std::string>> array_t::gen_string_vector(){
	std::vector<std::vector<std::string>> return_value;
	for(unsigned int i = 0;i < 3;i++){
		std::vector<std::string> the_tmp_vector;
		switch(i){
		case 0:
			the_tmp_vector = gen_int_array_vector();
			break;
		case 1:
			the_tmp_vector = gen_long_double_array_vector();
			break;
		case 2:
			the_tmp_vector = gen_string_array_vector();
			break;
		default:
			break;
		}
		return_value.push_back(the_tmp_vector);
	}
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
