#ifndef SERVER_MAIN_H
	#define SERVER_MAIN_H
	#include "../main.h"
	#include "../net/net_main.h"
	#include "../class/class_main.h"
	#include "server_class.h"
	#include "sstream"
	#include "fstream"
	#include "string"
	#include "cstdio"
	#include "vector"
	#include "cmath"
	extern int argc_;
	extern char** argv_;
	extern std::vector<client_extra_t> client_extra;
	extern std::vector<client_t*> client;
	extern std::vector<coord_extra_t> coord_extra;
	extern std::vector<coord_t*> coord;
	extern std::vector<model_extra_t> model_extra;
	extern std::vector<model_t*> model;
#endif
