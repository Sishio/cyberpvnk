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
	#define DEBUG_SEPERATOR 1
	#ifndef DEBUG_SEPERATOR
		#define ARRAY_ITEM_SEPERATOR_START		(char*)"\x01"
		#define ARRAY_ITEM_SEPERATOR_END		(char*)"\x02"
		#define ARRAY_TYPE_SEPERATOR_START		(char*)"\x03"
		#define ARRAY_TYPE_SEPERATOR_END		(char*)"\x04"
		#define ARRAY_INT_SEPERATOR_START		(char*)"\x05"
		#define ARRAY_INT_SEPERATOR_END			(char*)"\x06"
		#define ARRAY_LONG_DOUBLE_SEPERATOR_START	(char*)"\x07"
		#define ARRAY_LONG_DOUBLE_SEPERATOR_END		(char*)"\x08" // CAUTION: ASCII BACKSPACE
		#define ARRAY_STRING_SEPERATOR_START		(char*)"\x09"
		#define ARRAY_STRING_SEPERATOR_END		(char*)"\x0a"
		#define ARRAY_ID_START				(char*)"\x0b"
		#define ARRAY_ID_END				(char*)"\x0c"
		#define ARRAY_STARTING_START			(char*)"\x0d"
		#define ARRAY_STARTING_END			(char*)"\x0e"
		#define ARRAY_FUNCTION_START			(char*)"\x0f"
		#define ARRAY_FUNCTION_END			(char*)"\x10"
	#else
		// don't use this unless you can debug without using strings
		#define ARRAY_ITEM_SEPERATOR_START		(char*)"A"
		#define ARRAY_ITEM_SEPERATOR_END		(char*)"B"
		#define ARRAY_TYPE_SEPERATOR_START		(char*)"C"
		#define ARRAY_TYPE_SEPERATOR_END		(char*)"D"
		#define ARRAY_INT_SEPERATOR_START		(char*)"E"
		#define ARRAY_INT_SEPERATOR_END			(char*)"F"
		#define ARRAY_LONG_DOUBLE_SEPERATOR_START	(char*)"G"
		#define ARRAY_LONG_DOUBLE_SEPERATOR_END		(char*)"H"
		#define ARRAY_STRING_SEPERATOR_START		(char*)"I"
		#define ARRAY_STRING_SEPERATOR_END		(char*)"J"
		#define ARRAY_ID_START				(char*)"K"
		#define ARRAY_ID_END				(char*)"L"
		#define ARRAY_STARTING_START			(char*)"M"
		#define ARRAY_STARTING_END			(char*)"N"
		#define ARRAY_FUNCTION_START			(char*)"O"
		#define ARRAY_FUNCTION_END			(char*)"P"
	#endif
	#define ARRAY_STRING_HASH_BIT		0
	#define ARRAY_LONG_DOUBLE_HASH_BIT	1
	#define ARRAY_INT_HASH_BIT		2
	// used by the server to tell it what data it is allowed to modify because the clients can send fraud packets and allowing all of the packets would allow cheating
	// the networking code reserves 24-32 (packet seperators for serial connections among other things)
	class array_t{
	private:
		void parse_int_from_string(std::string);
		void parse_long_double_from_string(std::string);
		void parse_string_from_string(std::string);
		unsigned int pull_starting_number(std::string);
		std::vector<std::string> pull_items(char*, std::string, char*, std::vector<int>*);
		long int int_hash;
		long int string_hash;
		long int long_double_hash;
	public:
		void* pointer;
		std::string data_type;
		long double last_update;
		int id;
		std::vector<int*> int_array;
		std::vector<long double*> long_double_array;
		std::vector<std::string*> string_array;
		std::string gen_updated_string(int);
		std::string gen_long_double_string();
		std::string gen_int_string();
		std::string gen_string_string();
		array_t(void*); // TODO: put data_type in the initializer
		~array_t();
		bool id_match(int);
		void parse_string_entry(std::string);
		bool updated(int*);
	};
	//extern std::vector<array_t*> array_vector;
	extern long int array_scan_for_id(long int);
	extern void add_two_arrays(array_t*, array_t*);
	extern void update_class_data(std::string, int);
	extern void add_array_to_vector(array_t*);
	extern void delete_array_from_vector(array_t*);
	extern void delete_array_id(int);
	extern void* find_array_pointer(int);
	extern std::vector<array_t*> array_vector;
#endif
