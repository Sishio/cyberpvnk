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

static std::string wrap(char start, std::string data, char end){
	return std::to_string(start) + data + std::to_string(end);
}

array_t::array_t(int a){
	header = a;
	id = gen_rand();
}

bool array_t::id_match(int a){
	return id == a;
}

std::string array_t::gen_string(){
	static int int_size = int_array.size();
	std::string int_string = "";
	for(int i = 0;i < int_size;i++){
		int_string += wrap(ARRAY_ITEM_SEPARATOR_START,std::to_string(*int_array[i]),ARRAY_ITEM_SEPARATOR_END);
	}
	int_string = wrap(ARRAY_TYPE_SEPARATOR_START,int_string,ARRAY_TYPE_SEPARATOR_END);
	static int long_double_size = long_double_array.size();
	std::string long_double_string = "";
	for(int i = 0;i < long_double_size;i++){
		long_double_string += wrap(ARRAY_ITEM_SEPARATOR_START,std::to_string(*long_double_array[i]),ARRAY_ITEM_SEPARATOR_END);
	}
	long_double_string = wrap(ARRAY_TYPE_SEPARATOR_START,long_double_string,ARRAY_TYPE_SEPARATOR_END);
	static int string_size = string_array.size();
	std::string string_string = "";
	for(int i = 0;i < string_size;i++){
		string_string += wrap(ARRAY_ITEM_SEPARATOR_START,*string_array[i],ARRAY_ITEM_SEPARATOR_END);
	}
	string_string = wrap(ARRAY_TYPE_SEPARATOR_START,string_string,ARRAY_TYPE_SEPARATOR_END);
	return std::to_string(header) + int_string + long_double_string + string_string;
}

void array_t::parse_string(std::string a){
	int tmp_header = static_cast<int>(a[0]);
	if(tmp_header == header){
		std::string array[3] = {""};
		for(int i = 0;i < 3;i++){
			array[0] = a.substr(a.find_first_of(ARRAY_TYPE_SEPARATOR_START)+1,a.find_first_of(ARRAY_TYPE_SEPARATOR_END)-1);
			a = a.substr(a.find_first_of(ARRAY_TYPE_SEPARATOR_END)+1,sizeof(a.c_str()));
		}
		static int int_size = int_array.size();
		for(int i = 0;i < int_size;i++){
			std::string data = array[0].substr(array[0].find_first_of(ARRAY_ITEM_SEPARATOR_START),array[0].find_first_of(ARRAY_ITEM_SEPARATOR_END));
			*int_array[i] = atoi(data.c_str());
			array[0] = array[0].substr(array[0].find_first_of(ARRAY_ITEM_SEPARATOR_END)+1,sizeof(array[0].c_str()));
		}
		static int long_double_size = long_double_array.size();
		for(int i = 0;i < long_double_size;i++){
			std::string data = array[1].substr(array[i].find_first_of(ARRAY_ITEM_SEPARATOR_START),array[1].find_first_of(ARRAY_ITEM_SEPARATOR_END));
			*long_double_array[i] = atof(data.c_str());
			array[1] = array[1].substr(array[1].find_first_of(ARRAY_ITEM_SEPARATOR_END)+1,sizeof(array[1]));
		}
		static int string_size = string_array.size();
		for(int i = 0;i < string_size;i++){
			std::string data = array[2].substr(array[2].find_first_of(ARRAY_ITEM_SEPARATOR_START),array[2].find_first_of(ARRAY_ITEM_SEPARATOR_END));
			*string_array[i] = data;
			array[2] = array[2].substr(array[2].find_first_of(ARRAY_ITEM_SEPARATOR_END)+1,sizeof(data.c_str()));
		}
	}
}

void array_t::close(){
	int_array.clear();
	string_array.clear();
	long_double_array.clear();
}
