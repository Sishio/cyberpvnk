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

void loop_add(loop_t *a, void(*b)()){
	a->code.push_back(b);
}

void loop_run(loop_t *a, int settings){
	const unsigned long int code_size = a->code.size();
	if(CHECK_BIT(settings, LOOP_CODE_MT, 1)){
		std::vector<std::thread*> thread;
		for(unsigned long int i = 0;i < code_size;i++){
			thread.push_back(new std::thread(a->code[i]));
		}
		for(unsigned long int i = 0;i < code_size;i++){
			thread[i]->join();
			delete thread[i];
			thread[i] = nullptr;
		}
	}else{
		for(unsigned long int i = 0;i < code_size;i++){
			a->code[i]();
		}
	}
}

void loop_del(loop_t *a, void(*b)()){
	const unsigned long int code_size = a->code.size();
	for(unsigned long int i = 0;i < code_size;i++){
		if(a->code[i] == b){
			a->code.erase(a->code.begin()+i);
			break;
		}
	}
}
