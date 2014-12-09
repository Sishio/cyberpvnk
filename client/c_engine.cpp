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
#include "c_engine.h"

extern net_t *net;
extern client_t *self;

static void net_engine_parse(std::string *a){
}
static coord_t *old_coord = nullptr;

static net_ip_connection_info_t server_connection_info;

void net_engine(){
	server_connection_info.ip = "127.0.0.1";
	server_connection_info.port = 50000;
	std::string a;
	while((a = net->read()) != ""){
		net_engine_parse(&a);
	}
	if(old_coord == nullptr){
		old_coord = new coord_t;
	}
	if(memcmp(&old_coord,self->coord,sizeof(coord_t)) != 0){
		net->write(self->array->gen_string(), server_connection_info);
	}
	memcpy(&old_coord,self->coord,sizeof(coord_t)); //padding
}

void input_engine(){
}

void render_engine(){
}
