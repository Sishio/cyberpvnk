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
	#include "utility"
	#include "mutex"
	#include "thread"
	#define ARRAY_RESERVE_INT_SIZE 2 // spawn iteration and ID
	#define ARRAY_RESERVE_STRING_SIZE 1 // data_type
	#define ARRAY_RESERVE_LONG_DOUBLE_SIZE 0 // nothing needed here yet (spawn_time?)
	#define ARRAY_RESERVE_VOID_PTR_SIZE 1 // object
	#define ARRAY_VECTOR_SIZE 65536
	#define RESERVE_ID_SIZE 64
	#define RESERVE_ID_ITERATOR 0xB00B000000000000 // hehehe
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
			bool try_lock(){return true;}
		};
	#endif
	#define ID_BIT_LENGTH 16
	#define ARRAY_SETTING_SEND 1
	#define ARRAY_SETTING_WRITE_PROTECTED 2
	#define ARRAY_SETTING_IMMUNITY 4
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
		int_ settings_;
	public:
		// directly related to the pointer item
		void* pointer;
		std::string data_type;
		std::string name;
		// directly related to the array iteself
		bool put_inside_type_vector;
		array_id_t id;
		array_t(void*, std::string, int_); // TODO: put data_type in the initializer
		~array_t();
		lock_t data_lock;
		lock_t int_lock, long_double_lock, string_lock,  void_ptr_lock;
		std::vector<std::pair<int_*, std::string> > int_array;
		std::vector<std::pair<long double*, std::string> > long_double_array;
		std::vector<std::pair<std::string*, std::string> > string_array;
		std::vector<std::pair<void**, std::string> > void_ptr_array; // don't network this, just use it for console things
		void int_in_bounds(uint_ a){
			if(a >= int_array.size()){
				throw std::out_of_range("int");
			}
		}
		void long_double_in_bounds(uint_ a){
			if(a >= long_double_array.size()){
				throw std::out_of_range("long double");
			}
		}
		void string_in_bounds(uint_ a){
			if(a >= string_array.size()){
				throw std::out_of_range("string");
			}
		}
		void void_ptr_in_bounds(uint_ a){
			if(a >= void_ptr_array.size()){
				throw std::out_of_range("void_ptr");
			}
		}
		// adders need the locks
		void set_lock(lock_t *tmp){
			data_lock.lock();
			tmp->lock();
		}
		void unset_lock(lock_t *tmp){
			data_lock.unlock();
			tmp->unlock();
		}
		void add_int(std::pair<int_*, std::string> a){
			set_lock(&int_lock);
			int_array.push_back(a);
			unset_lock(&int_lock);
		}
		void add_long_double(std::pair<long double*, std::string> a){
			set_lock(&long_double_lock);
			long_double_array.push_back(a);
			unset_lock(&long_double_lock);
		}
		void add_string(std::pair<std::string*, std::string> a){
			set_lock(&string_lock);
			string_array.push_back(a);
			unset_lock(&string_lock);
		}
		void add_void_ptr(std::pair<void**, std::string> a){
			set_lock(&void_ptr_lock);
			void_ptr_array.push_back(a);
			unset_lock(&void_ptr_lock);
		}
		// so do the setters (the pointer shouldn't change, so these really shouldn't be needed)
		void set_int(uint_ a, int_ b){
			set_lock(&int_lock);
			int_in_bounds(a);
			*(std::get<0>(int_array[a])) = b;
			unset_lock(&int_lock);
		}
		void set_long_double(uint_ a, long double b){
			set_lock(&long_double_lock);
			long_double_in_bounds(a);
			*(std::get<0>(long_double_array[a])) = b;
			unset_lock(&long_double_lock);
		}
		void set_string(uint_ a, std::string b){
			set_lock(&string_lock);
			string_in_bounds(a);
			*(std::get<0>(string_array[a])) = b;
			unset_lock(&string_lock);
		}
		void set_void_ptr(uint_ a, void* b){
			set_lock(&void_ptr_lock);
			void_ptr_in_bounds(a);
			*(std::get<0>(void_ptr_array[a])) = b;
			unset_lock(&void_ptr_lock);
		}
		// receive pointer to the object (useful for testing if an object exists)
		int get_int(uint_ a){
			set_lock(&int_lock);
			int_in_bounds(a);
			int retval = *(std::get<0>(int_array[a]));
			unset_lock(&int_lock);
			return retval;
		}
		long double get_long_double(uint_ a){
			set_lock(&long_double_lock);
			long_double_in_bounds(a);
			long double retval = *(std::get<0>(long_double_array[a]));
			unset_lock(&long_double_lock);
			return retval;
		}
		std::string get_string(uint_ a){
			set_lock(&string_lock);
			string_in_bounds(a);
			std::string retval = *(std::get<0>(string_array[a]));
			unset_lock(&string_lock);
			return retval;
		}
		void* get_void_ptr(uint_ a){
			set_lock(&void_ptr_lock);
			void_ptr_in_bounds(a);
			void* retval = *(std::get<0>(void_ptr_array[a]));
			unset_lock(&void_ptr_lock);
			return retval;
		}
		std::string gen_updated_string(int_ what_to_update = ~0);
		std::string gen_long_double_string();
		std::string gen_int_string();
		std::string gen_string_string();
		bool updated(int_*);
		void update_data();
		// getter and setter functions
		// DEPRECATED. Try and use the get_settings and set settings
		bool get_send(){return settings_&ARRAY_SETTING_SEND;}
		bool get_write_protected(){return settings_&ARRAY_SETTING_WRITE_PROTECTED;}
		bool get_immunity(){return settings_&ARRAY_SETTING_IMMUNITY;}
		// end dep.
		bool get_setting(int_ x){return (settings_&x)!=0;} //comparing against 0 is pretty fast
		void set_setting(int_ x, bool y){(y)?(settings_|=x):(settings_&=~x);}
		void set_settings(int_ a){settings_=a;}
		int_ get_settings(){return settings_;}
		void new_id(array_id_t);

		// debugging
		bool unlocked();
		std::string gen_print_prefix();
		void reset_values();
		long double last_update;
		bool id_match(array_id_t);
		void parse_string_entry(std::string);
		std::string print();
	};
	class array_sync_t{
	public:
		array_t array;
		array_id_t array_vector_[ARRAY_VECTOR_SIZE];
		array_sync_t();
		~array_sync_t();
		void read();
		void write();
	};
	//extern std::vector<array_t*> array_vector;
	extern array_id_t array_scan_for_id(array_id_t);
	extern void add_two_arrays(array_t*, array_t*);
	extern void update_class_data(std::string, int_);
	extern void add_array_to_vector(array_t*);
	extern void delete_array_from_vector(array_t*);
	extern void delete_array_id(array_id_t);
	extern void delete_all_data();
	extern void delete_array_and_pointer(array_id_t);
	extern void* find_pointer(array_id_t, std::string type = "");
	extern array_t* find_array_pointer(array_id_t);
	extern array_id_t array_highest_id();
	extern array_t* array_vector[ARRAY_VECTOR_SIZE];
	extern lock_t array_lock;
	extern void lock_array_of_data_type(std::vector<void*>, std::string);
	extern std::vector<array_id_t> all_ids_of_type(std::string);
	extern void delete_thread(std::thread*);
	extern bool valid_id(array_id_t);
	extern std::vector<std::string> generate_outbound_class_data();
	extern void update_all_array_entries();
	#include "../input/input_main.h"
#endif
