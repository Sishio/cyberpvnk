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
	#include "../class/class_main.h"
	#include "../class/class_array.h"
	#include "../loop/loop_main.h"
	#include "sstream"
	#include "fstream"
	#include "string"
	#include "cstdio"
	#include "vector"
	#include "cmath"
	class server_info_t{
	public:
		std::string map_name, map_path;
		server_info_t();
		~server_info_t();
	};
	extern int_ argc_;
	extern char** argv_;
	extern loop_t *server_loop_code;
#endif
