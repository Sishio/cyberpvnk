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
#ifndef SERVER_CONSOLE_H
	#define SERVER_CONSOLE_H
	#include "../class/class_main.h"
	#include "../util/util_main.h"
	#include "../net/net_main.h"
	extern void console_init();
	extern void console_parse();
	extern void console_engine();
	extern void console_close();
#endif
