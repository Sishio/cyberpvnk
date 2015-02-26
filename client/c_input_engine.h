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

#include "../class/class_main.h"
#include "../class/class_array.h"
#include "../input/input_main.h"
#include "c_net_engine.h"
#include "c_render_engine.h"
#ifndef C_INPUT_ENGINE
	#define C_INPUT_ENGINE	
	extern input_t *input;
	void input_init();
	void input_engine();
	void input_close();
#endif
