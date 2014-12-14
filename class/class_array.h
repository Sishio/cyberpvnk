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
#ifndef CLASS_ARRAY_H
	#define CLASS_ARRAY_H
	#include "../util/util_main.h"
	#include "cstdio"
	#include "string"
	#include "cstdio"
	#include "cstdlib"
	#include "vector"
	#define ARRAY_ITEM_SEPERATOR_START	(char*)"a"
	#define ARRAY_ITEM_SEPERATOR_END	(char*)"b"
	#define ARRAY_TYPE_SEPERATOR_START	(char*)"c"
	#define ARRAY_TYPE_SEPERATOR_END	(char*)"d"
	#define ARRAY_INT_SEPERATOR_START	(char*)"e"		// item start and type end
	#define ARRAY_INT_SEPERATOR_END		(char*)"f"		// type end and item start
	#define ARRAY_LONG_DOUBLE_SEPERATOR_START	(char*)"g"	// item end and item end
	#define ARRAY_LONG_DOUBLE_SEPERATOR_END		(char*)"h"	// item start and item start
	#define ARRAY_STRING_SEPERATOR_START		(char*)"i"	// type end and type end
	#define ARRAY_STRING_SEPERATOR_END		(char*)"j"	// type start and type start
	#define ARRAY_ID_START			(char*)"k"
	#define ARRAY_ID_END			(char*)"l"
	#define ARRAY_STARTING_START	(char*)"m"
	#define ARRAY_STARTING_END	(char*)"n"

	/*#define ARRAY_ITEM_SEPERATOR_START	(char*)"\x17\x02\x31"
	#define ARRAY_ITEM_SEPERATOR_END	(char*)"\x17\x03\x31"
	#define ARRAY_TYPE_SEPERATOR_START	(char*)"\x17\x01\x31"
	#define ARRAY_TYPE_SEPERATOR_END	(char*)"\x17\x04\x31"
	#define ARRAY_INT_SEPERATOR_START	(char*)"\x17\x02\x04\x31"		// item start and type end
	#define ARRAY_INT_SEPERATOR_END		(char*)"\x17\x04\x02\x31"		// type end and item start
	#define ARRAY_LONG_DOUBLE_SEPERATOR_START 	(char*)"\x17\x03\x03\x31"	// item end and item end
	#define ARRAY_LONG_DOUBLE_SEPERATOR_END		(char*)"\x17\x02\x02\x31"	// item start and item start
	#define ARRAY_STRING_SEPERATOR_START		(char*)"\x17\x04\x04\x31"	// type end and type end
	#define ARRAY_STRING_SEPERATOR_END		(char*)"\x17\x01\x01\x31"	// type start and type start
	#define ARRAY_ID_START			(char*)"\x17\x05\x31"
	#define ARRAY_ID_END			(char*)"\x17\x05\x05\x31"
	#define ARRAY_STARTING_START	(char*)"\x17\x04\x05\x31"
	#define ARRAY_STARTING_END	(char*)"\x17\x05\x04\x31"*/

	class array_t{
	private:
		std::vector<std::string> gen_int_array_vector();
		std::vector<std::string> gen_long_double_array_vector();
		std::vector<std::string> gen_string_array_vector();
		void parse_int_from_string(std::string);
		void parse_long_double_from_string(std::string);
		void parse_string_from_string(std::string);
		unsigned int pull_starting_number(std::string);
		std::vector<std::string> pull_items(char*, std::string, char*);
	public:
		int id;
		std::vector<int*> int_array;
		std::vector<long double*> long_double_array;
		std::vector<std::string*> string_array;
		array_t();
		bool id_match(int);
		std::vector<std::vector<std::string>> gen_string_vector();
		void parse_string_entry(std::string);
		void parse_string_vector(std::vector<std::vector<std::string>>);
		void close();
	};
	extern void update_data(std::string);
	extern array_t* new_array();
	extern void delete_array(array_t*);
#endif
