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
#include "../loop/loop_main.h"
#include "../class/class_main.h"
#include "../net/net_main.h"
#ifndef C_NET_ENGINE_H
	#define C_NET_ENGINE_H
	#include "../client/c_main.h"
	#include "../net/net_const.h"
	extern net_t *net;
	extern array_id_t host_info_id;
	extern array_id_t self_info_id;
	extern void net_init();
	extern void net_engine();
	extern void net_close();
#endif
