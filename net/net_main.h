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
#ifndef NET_H
	#define NET_H
	class net_t{
	private:
		std::thread send_thread;
		std::thread read_thread;
	public:
		int init_parse_parameters(int,char**);
		int init_initialize_subsystems(int, char**, int);
		std::string encode_data(unsigned char, void*);
		net_ip_t *ip;
		void blank();
		net_t(int, char**, int);
		int loop();
		~net_t();
		std::string read(std::string search = "");
		void write(std::string, int, unsigned long int);
	};
	extern std::string net_generate_ip_address(std::string prefix = "");
	extern void net_write_array_vector(std::vector<std::vector<std::string>>, int);
	extern void net_loop(net_t*);
#endif
