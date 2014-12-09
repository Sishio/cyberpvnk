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
#ifndef NET_STORE_H
	#define NET_STORE_H
	#include "../class/class_main.h"
	#include "vector"
	extern void store_coord_t(coord_t, std::vector<coord_t*>*, std::vector<coord_extra_t>*);
	extern void store_model_t(model_t, std::vector<model_t*>*, std::vector<model_extra_t>*);
#endif

