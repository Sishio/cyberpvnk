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
#ifndef SERVER_MAIN_H
	#define SERVER_MAIN_H
	#include "../main.h"
	#include "../net/net_main.h"
	#include "../class/class_main.h"
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
