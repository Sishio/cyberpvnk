#include "render_main.h"

render_data_t::render_data_t(){
	screen = nullptr;
	coord = nullptr;
	rules = new render_rules_t();
}

render_data_t::~render_data_t(){
	if(screen != nullptr){
		printf("Deleting render_data_t with a non-null screen, this isn't good\n");
	}
	coord = nullptr;
	delete rules;
	rules = nullptr;
}
