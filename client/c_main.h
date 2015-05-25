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
#ifndef C_MAIN_H
	#define C_MAIN_H
	#include "../loop/loop_main.h"
	#include "../main.h"
	#include "../net/net_main.h"
	#include "../input/input_main.h"
	#include "../util/util_main.h"
	#include "../class/class_main.h"
	#include "../math/math_main.h"
	#include "cstdio"
	#ifdef __linux
		#include "signal.h"
	#endif
	extern loop_t loop;
	extern bool terminate;
	extern bool once_per_second;
	extern array_id_t self_id;
	extern int_ argc_;
	extern char** argv_;
	extern int_ menu_loop();
#endif
