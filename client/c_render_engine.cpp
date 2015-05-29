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

extern int_ argc_;
extern char **argv_;

render_t *render = nullptr;

coord_t *background_coord;

void background_init(){
	background_coord = new coord_t;
	if(background_coord == nullptr){
		printf_(NEW_NULLPTR_ERROR, PRINTF_ERROR);
		return;
	}
	image_t *background_image = new image_t("../image_data/ui/bg.png");
	if(background_image == nullptr){
		printf_(NEW_NULLPTR_ERROR, PRINTF_ERROR);
		return;
	}
	tile_t *background_tile = new tile_t;
	if(background_tile == nullptr){
		printf_(NEW_NULLPTR_ERROR, PRINTF_ERROR);
		return;
	}
	for(uint_ i = 0;i < 128;i++){
		background_tile->set_image_id(i, background_image->array.id);
	}
	background_coord->tile_id = background_tile->array.id;
}

void render_init(){
	loop_add(&loop, "render_engine", render_engine);
	render = new render_t(argc_, argv_);
	screen_t *screen_ = new screen_t();
	screen_->x_res = 640;
	screen_->y_res = 480;
	screen_->title = "Cyberpvnk";
	screen_->new_screen();
	render->screen.push_back(screen_->array.id);
	background_init();
}

long double old_time = 0;

void render_engine(){
	long double curr_time = get_time();
	if(old_time == 0) old_time = curr_time;
	background_coord->x += 10*(curr_time-old_time);
	background_coord->y += 10*(curr_time-old_time);
	old_time = curr_time;
	if(render != nullptr){
		render->loop();
	}
}

void render_close(){
	loop_del(&loop, render_engine);
}
