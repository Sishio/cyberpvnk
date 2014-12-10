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
#ifndef NET_IP_H
	#define NET_IP_H
	#define NET_IP_MTU 256
	#define NET_IP_CONNECTION_TYPE_UNDEFINED 0
	#define NET_IP_CONNECTION_TYPE_UDP 1
	#define NET_IP_CONNECTION_TYPE_TCP 2
	#define NET_IP_SERVER_RECEIVE_PORT 50000
	#define NET_IP_CLIENT_RECEIVE_PORT 50001
	#include "cstdio"
	#include "vector"
	#include "string"
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_net.h"
	class net_ip_connection_info_t{
	public:
		net_ip_connection_info_t();
		char connection_type; // UDP or TCP
		std::string ip;
		unsigned short int port;
		// TCP socket when that is implemented
	};
	class net_ip_info_t{
	public:
		void blank();
		char* server_ip;
		int server_port;
		int client_port;
		int mtu;
	};
	struct net_ip_write_buffer_t{
		unsigned short int port;
		std::string ip;
		std::string data;
	};
	class net_ip_t{
	private:
		net_ip_info_t ip_info;
		//std::string *read_buffer,*write_buffer;
		std::vector<std::string> read_buffer;
		std::vector<net_ip_write_buffer_t> write_buffer;
		UDPsocket outbound;
		UDPsocket inbound; // no TCP yet
		UDPpacket *outbound_packet;
		UDPpacket *inbound_packet;
	public:
		net_ip_connection_info_t connection_info;
		void blank();
		void set_receive_port(unsigned short int);
		int init(int,char**);
		std::string read();
		int write(std::string, net_ip_connection_info_t);
		void send_now(net_ip_write_buffer_t);
		std::string receive_now();
		void loop();
		void loop_send();
		void loop_receive();
		void close();
	};
#endif
