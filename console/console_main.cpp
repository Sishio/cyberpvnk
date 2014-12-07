// This is a network console I wrote for the client and for the server

#include "cstdio"
#include "string"
#include "../net/net_main.h"

int main(int argc, char **argv){
	net_t *net = new net_t;
	net->init(argc,argv);
	while(true){
		net->loop();
		printf("]");
		char tmp[101];
		scanf("%100s", tmp);
		std::string a = tmp;
		net->write(a, 50010, "127.0.0.1");
	}
	net->close();
	delete net;
	net = NULL;
	return 0;
}
