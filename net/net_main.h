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
#ifndef NET_H
	#define NET_H
	#include "../main.h"
	#include "../class/class_main.h"
	#include "net_ip.h"
	#include "net_const.h"
	#include "net_store.h" // stores the data
	#include "iostream"
	#include "utility"
	#include "chrono"
	#include "functional"
	#include "atomic"
	#include "thread"
	#include "../util/util_main.h"
	class net_t{
	private:
		std::thread send_thread;
		std::thread read_thread;
		uint_ packet_id_count;
	public:
		int_ init_parse_parameters(int_,char**);
		int_ init_initialize_subsystems(int_, char**, int_);
		std::string encode_data(unsigned char, void*);
		net_ip_t *ip;
		void blank();
		net_t(int_, char**, int_);
		int_ loop();
		~net_t();
		std::string read(std::string search = "");
		void write(std::string data, int_ a, uint_ packet = 0);
	};
	extern std::string net_generate_ip_address(std::string prefix = "");
	extern void net_write_array_vector(std::vector<std::vector<std::string>>, int_);
	extern void net_loop(net_t*);
#endif
