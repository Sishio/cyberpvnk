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
	#include "net_serial.h"
	#include "net_ip.h"
	#include "net_store.h" // stores the data
	#include "thread"
	#include "../util/util_main.h"
	#define NET_PACKET_COORD_T_ID 0 // any single char*acter would work fine.
	#define NET_PACKET_MODEL_T_ID 1
	#define NET_MAJOR_SEPERATOR '\027' // end of trans. block
	#define NET_MINOR_SEPERATOR '\037' // unit seperator
	#define NET_SCANF_COORD_T "%d %d %f %f %f %f %f" // type, id, x,y,z,x_angle,y_angle
	#define NET_SCANF_MODEL_T "%d %d %s %d" // type, id, alias, type (the data is sent seperately)
	#define NET_MTU 512 // chunk the data (used in the encoding and decoding code).
	// major seperaters seperate packets from each other and minor seperators seperate items from inside of a packet
	#define NET_COORD 0
	#define NET_MODEL 1
	#define NET_MAJOR_START '\01'
	#define NET_MINOR_START '\02'
	#define NET_MAJOR_END '\04'
	#define NET_MINOR_END '\03'
	class net_t{
	public:
		int init_parse_parameters(int,char**);
		int init_initialize_subsystems(int,char**);
		std::string encode_data(unsigned char, void*);
		net_serial_t *serial;
		net_ip_t *ip;
		void blank();
		net_t(int,char**);
		int loop();
		void close();
		std::string read();
		int write(std::string, net_ip_connection_data_t);
		int write(std::string, net_serial_connection_data_t);
	};
	extern void net_loop(net_t*);
#endif
