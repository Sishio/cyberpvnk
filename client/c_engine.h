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
#ifndef C_ENGINE_H
	#define C_ENGINE_H
	#include "c_main.h"
	extern void net_init();
	extern void net_engine();
	extern void net_close();
	extern void input_init();
	extern void input_engine();
	extern void input_close();
	extern void render_init();
	extern void render_engine();
	extern void render_close();
	extern net_t *net;
	extern render_t *render;
	extern input_t *input;
	extern client_t *self;
	extern std::vector<coord_t*> coord;
	extern std::vector<coord_extra_t> coord_extra;
	extern std::vector<model_t*> model;
	extern std::vector<model_extra_t> model_extra;
	extern int argc_;
	extern char** argv_;
#endif
