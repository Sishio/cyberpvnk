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

#include "loop_main.h"

loop_t::loop_t(){
  target_rate = 60;
	settings = 0;
}

void loop_add(loop_t *a, std::string name, void(*b)()){
	loop_entry_t tmp;
	tmp.code = b;
	tmp.name = name;
	a->code.push_back(tmp);
}

void loop_run(loop_t *a, int settings){
	long double end_time, start_time;
	std::string summary = a->name + "\n";
	const unsigned long int code_size = a->code.size();
	const bool print_this_time = (CHECK_BIT(settings, LOOP_OPS_PRINT, 1) && once_per_second) || CHECK_BIT(settings, LOOP_OPS_PRINT, 0);
	start_time = get_time();
	if(CHECK_BIT(settings, LOOP_CODE_MT, 1)){
		std::vector<std::thread*> thread;
		for(unsigned long int i = 0;i < code_size;i++){
			thread.push_back(new std::thread(a->code[i].code));
		}
		for(unsigned long int i = 0;i < code_size;i++){
			thread[i]->join();
			delete thread[i];
			thread[i] = nullptr;
		}
	}else{
		for(unsigned long int i = 0;i < code_size;i++){
			const long double start_time = get_time();
			a->code[i].code();
			if(print_this_time){
				summary += "\t" + a->code[i].name + "\t\t" + std::to_string(get_time()-start_time) + "\n";
			}
		}
	}
	end_time = get_time();
	const long double current_rate = 1/(end_time-start_time);
	const long double additional_time = -(end_time + (a->target_rate*start_time));
	if(additional_time > 0){
		ms_sleep(additional_time*1000);
	}else{
		SET_BIT(&a->settings, LOOP_OPS_PRINT, 1); // 0: print every iteration, 1: print once per second
	}
	summary += "current frame rate: " + std::to_string(current_rate) + "\n";
	if(print_this_time){
		printf("%s", summary.c_str());
	}
}

void loop_del(loop_t *a, void(*b)()){
	const unsigned long int code_size = a->code.size();
	for(unsigned long int i = 0;i < code_size;i++){
		if(a->code[i].code == b){
			a->code.erase(a->code.begin()+i);
			break;
		}
	}
}
