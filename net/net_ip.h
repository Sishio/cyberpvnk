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
//#define ALPHA_NEGATIVE_SEPERATOR 1
#include "net_const.h"
#include "cstdio"
#include "cstdlib"
#include "vector"
#include "mutex"
#include "string"
#include "thread"
#if defined(__linux) || defined(OSX)
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_net.h"
#else
	#include "SDL.h"
	#include "SDL_net.h"
#endif
#ifndef NET_IP_H
	#define NET_IP_H
	#define NET_IP_MTU 256
	#define NET_IP_CONNECTION_TYPE_UNDEFINED	0
	#define NET_IP_CONNECTION_TYPE_UDP		1
	#define NET_IP_CONNECTION_TYPE_TCP		2
	#define NET_IP_SERVER_RECEIVE_PORT		50000
	#define NET_IP_CLIENT_RECEIVE_PORT		50001
	// refer to net_const.h for how the special characters are delegated
	#ifdef DEBUG_SEPERATOR
		#define NET_IP_RETURN_ADDRESS_START	(char*)"Z"
		#define NET_IP_RETURN_ADDRESS_END	(char*)"Y"
	#elif ALPHA_NEGATIVE_SEPERATOR
		#define NET_IP_RETURN_ADDRESS_START		-30
		#define NET_IP_RETURN_ADDRESS_END		-31
	#else
		#define NET_IP_RETURN_ADDRESS_START		(char*)"\x1e"
		#define NET_IP_RETURN_ADDRESS_END		(char*)"\x1f"
	#endif
	class net_ip_write_buffer_t{
	public:
		array_t array; // just used for locks
		bool sent;
		uint_ position;
		int_ connection_info_id;
		net_ip_write_buffer_t(std::string, net_packet_id, int_);
		std::vector<std::string> gen_string_vector();
		uint_ packet_id;
		std::string data;
	};
	class net_ip_read_buffer_t{
	public:
		array_t array;
		net_ip_read_buffer_t(net_packet_id);
		~net_ip_read_buffer_t();
		void parse_packet_segment(std::string);
		bool finished();
		std::string gen_string();
		uint_ packet_id;
		std::vector<std::string> data_vector;
	};
	struct udp_socket_t{
		array_id_t connection_info_id;
		UDPsocket socket;
		udp_socket_t(array_id_t);
		UDPsocket get_socket();
		~udp_socket_t();
	};
	class net_ip_t{
	private:
		std::mutex read_buffer_lock;
		std::mutex write_buffer_lock;
		uint_ total_sent_bytes;
		UDPpacket *outbound_packet;
		udp_socket_t *outbound; // no TCP yet
		UDPpacket *inbound_packet;
		udp_socket_t *inbound; // switching on the spot without losing data
		bool receive_check_read_array(std::string, uint_);
		void update_outbound_port(array_id_t);
		void update_inbound_port(array_id_t);
	public:
		array_t array;
		net_ip_t();
		int_ new_read_buffer;
		int_ new_write_buffer;
		net_ip_connection_info_t self_info;
		int_ init();
		void set_inbound_info(array_id_t);
		std::string read(std::string);
		void write(std::string, int_, uint_);
		void write_array_vector(std::vector<std::string>, uint_); // net_ip_connection_info ID
		int_ send_now(net_ip_write_buffer_t*);
		std::string receive_now();
		void loop();
		void loop_send();
		void loop_receive();
		void close();
	};
	extern std::string pull_global_ip_address();
	extern std::string pull_local_ip_address(std::string local_ip = "");
	extern std::string net_gen_ip_address(std::string);// gen local if local, global if global, and loopback if loopback
#endif
