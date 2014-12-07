#ifndef NET_SERIAL_H
	#define NET_SERIAL_H
	#include "cstdio"
	#include "iostream"
	class net_serial_t{
	public:
		void blank();
		void init(int,char**);
		void loop();
		void close();
		std::string read();
		void write(std::string);
	};
#endif
