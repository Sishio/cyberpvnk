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
#include "c_render_engine.h"

render_t *render = nullptr;
extern array_id_t self_id;

static array_id_t old_self_id = 0;
static array_id_t old_coord_id = 0;
static client_t *self = nullptr;
static coord_t *coord = nullptr;

extern int argc_;
extern char **argv_;

static void update_pointers(){
	if(unlikely(self_id != old_self_id)){
		self = (client_t*)find_pointer(self_id, "client_t");
		if(self != nullptr){
			old_self_id = self_id;
			if(likely(self->coord_id != old_coord_id)){
				coord = (coord_t*)find_pointer(self->coord_id, "coord_t");
				old_coord_id = self->coord_id;
			}
		}
	}
}

void render_init(){
	update_pointers();
	render = new render_t(argc_, argv_);
	loop_add(&loop, "render_engine", render_engine);
}

void render_engine(){
	if(unlikely(render == nullptr)){
		render_init();
	}
	update_pointers();
	render->loop(coord);
}

void render_close(){
	loop_del(&loop, render_engine);
	update_pointers(); // why not?
}
