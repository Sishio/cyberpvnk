#ifndef NET_IP_H
	#define NET_IP_H
	#define NET_IP_CLIENT_PORT 50000
	#define NET_IP_MTU 256
	#include "cstdio"
	#include "vector"
	#include "string"
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_net.h"
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
		UDPsocket inbound; // no TCP
		UDPpacket *outbound_packet;
		UDPpacket *inbound_packet;
	public:
		void blank();
		void set_receive_port(unsigned short int);
		int init(int,char**);
		std::string read();
		int write(std::string, unsigned short int, std::string);
		void send_now(net_ip_write_buffer_t);
		std::string receive_now();
		void loop();
		void loop_send();
		void loop_receive();
		void close();
	};
#endif
