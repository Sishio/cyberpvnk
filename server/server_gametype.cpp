#include "server_main.h"
#include "server_gametype.h"

gametype_t::gametype_t() : array(this, true){}

void gametype_t::engine(){}

gametype_t::~gametype_t(){}

gametype_info_t::gametype_info_t() : array(this, true){
	array.int_array.push_back(&gametype_id);
}

gametype_info_t::~gametype_info_t(){}

static gametype_info_t gametype_info;

void gametype_engine();

void gametype_init(){
	loop_add(&server_loop_code, "gametype engine", gametype_engine);
}

void gametype_engine(){
	gametype_t *current_gametype = (gametype_t*)find_pointer(gametype_info.gametype_id, "gametype_t");
	if(current_gametype == nullptr){
		current_gametype->engine();
	}
}

void gametype_close(){
	loop_del(&server_loop_code, gametype_engine);
}
