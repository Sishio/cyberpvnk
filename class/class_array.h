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
	#include "../main.h"
	#include "cstdint"
	typedef int_ array_id_t;
	#include "../util/util_main.h"
	#ifdef __linux
		#include "signal.h"
	#endif
	#include "cstdio"
	#include "iostream"
	#include "cstdint"
	#include "string"
	#include "cstdio"
	#include "cstdlib"
	#include "vector"
	#define ARRAY_RESERVE_INT_SIZE 2 // spawn iteration and ID
	#define ARRAY_RESERVE_STRING_SIZE 1 // data_type
	#define ARRAY_RESERVE_LONG_DOUBLE_SIZE 0 // nothing needed here yet (spawn_time?)
	#define ARRAY_VECTOR_SIZE 65536
	#define RESERVE_ID_SIZE 64
	#define RESERVE_ID_ITERATOR 0xB00B0000 // hehehe
	//#define DEBUG_SEPERATOR 1
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
	#ifdef MULTITHREAD
		typedef std::mutex lock_t;
	#else
		class lock_t{
		public:
			void lock(){}
			void unlock(){}
		};
	#endif
	#define ID_BIT_LENGTH 16
	class array_t{
	private:
		void parse_int_from_string(std::string);
		void parse_long_double_from_string(std::string);
		void parse_string_from_string(std::string);
		uint_ pull_starting_number(std::string);
		std::vector<std::string> pull_items(char*, std::string, char*, std::vector<int>*);
		int_ int_hash;
		int_ string_hash;
		int_ long_double_hash;
		std::vector<int> int_new;
		std::vector<int> double_new;
		std::vector<int> string_new;
		int_ send;
		int_ write_protected;
	public:
		bool unlocked();
		bool get_write_protected();
		bool get_send();
		std::string gen_print_prefix(); // don't store this as a value since copies would not change the initial address
		void new_id(array_id_t);
		std::mutex data_lock;
		std::mutex int_lock, long_double_lock, string_lock;
		void update_pointers();
		void reset_values();
		void* pointer;
		std::string data_type;
		long double last_update;
		array_id_t id;
		std::vector<int_*> int_array;
		std::vector<long double*> long_double_array;
		std::vector<std::string*> string_array;
		std::string gen_updated_string(int_ what_to_update = ~0);
		std::string gen_long_double_string();
		std::string gen_int_string();
		std::string gen_string_string();
		array_t(void*, bool add); // TODO: put data_type in the initializer
		~array_t();
		bool id_match(array_id_t);
		void parse_string_entry(std::string);
		bool updated(int_*);
		std::string print();
		void immunity(bool);
		int_ new_int();
		void delete_int(int_);
		void delete_long_double(int_);
		void delete_string(int_);
		int_ new_long_double();
		int_ new_string();
	};
	//extern std::vector<array_t*> array_vector;
	extern array_id_t array_scan_for_id(array_id_t);
	extern void add_two_arrays(array_t*, array_t*);
	extern void update_class_data(std::string, int_);
	extern void add_array_to_vector(array_t*);
	extern void delete_array_from_vector(array_t*);
	extern void delete_array_id(array_id_t);
	extern void delete_all_data();
	extern void delete_array_and_pointer(array_t*);
	extern __attribute__((always_inline)) void* find_pointer(array_id_t, std::string type = "");
	extern __attribute__((always_inline)) array_t* find_array_pointer(array_id_t);
	extern array_id_t array_highest_id();
	extern array_t* array_vector[ARRAY_VECTOR_SIZE];
	extern std::mutex array_lock;
	extern void lock_array_of_data_type(std::vector<void*>, std::string);
	extern std::vector<array_id_t> all_ids_of_type(std::string);
	extern void delete_thread(std::thread*);
	extern bool valid_id(array_id_t);
	extern std::vector<std::string> generate_outbound_class_data();
	#include "../input/input_main.h"
#endif
