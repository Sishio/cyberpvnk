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

#include "../class/class_array.h"
#include "loop_main.h"

loop_t::loop_t() : array(this, "loop_t", ARRAY_SETTING_IMMUNITY){
	target_rate = average_rate = 0;
	settings = 0;
	tick = 0;
	array.long_double_array.push_back(std::make_pair(&target_rate, "target rate"));
	array.long_double_array.push_back(std::make_pair(&average_rate, "average_rate"));
	array.int_array.push_back(std::make_pair(&settings, "settings"));
	array.int_array.push_back(std::make_pair(&tick, "tick"));
	array.data_type = "loop_t";
}

static void stop_infinite_loop(loop_entry_t *tmp){
	tmp->term = true;
	tmp->thread->join();
	delete tmp->thread;
	tmp->thread = nullptr;
}

static void infinite_loop_function(void(*code)(), bool *term_pointer){
	while(likely(infinite_loop() && *term_pointer == false)){
		code();
	}
}

loop_entry_t::loop_entry_t(){
	iteration_skip = 0;
	code = nullptr;
	settings = 0;
	thread = nullptr;
	term = false;
}

bool loop_entry_t::get_settings(int_ settings_){
	return (settings & settings_) == settings_;
}

void loop_entry_t::set_settings(int_ settings_){
	settings |= settings_;
}
void loop_add(loop_t *a, loop_entry_t b){
	a->code.push_back(b);
}

static bool loop_entry_will_run(const loop_t *loop, const loop_entry_t *loop_entry){
	if(loop_entry->iteration_skip == 0){
		return true;
	}else{
		if(loop->tick%(loop_entry->iteration_skip+1) == 0){
			return loop->tick%(loop_entry->iteration_skip+1);
		}
	}
	return false;
}

void loop_run(loop_t *a){
	if((a->settings & LOOP_PRINT_THIS_TIME) != 0){
		printf_("settings: " + std::to_string(a->settings) + "\n", PRINTF_VITAL);
	}
	std::string summary = a->array.name + "\n";
	const uint_ code_size = a->code.size();
	const long double start_time = get_time();
	for(uint_ i = 0;i < a->code.size();i++){
		if(a->code[i].term == true){
			if(a->code[i].thread != nullptr){
				stop_infinite_loop(&(a->code[i]));
			}else{
				a->code.erase(a->code.begin()+i);
			}
		}else if(a->code[i].code != nullptr){ // Deleting its place in the array allows for a seg fault if both pieces run
			if(a->code[i].get_settings(LOOP_CODE_PARTIAL_MT)){
				if(a->code[i].thread != nullptr){
					stop_infinite_loop(&(a->code[i]));
				}
				a->code[i].thread = new std::thread(a->code[i].code);
			}else if(a->code[i].get_settings(LOOP_CODE_NEVEREND_MT)){
				if(a->code[i].thread == nullptr){
					a->code[i].thread = new std::thread(infinite_loop_function, a->code[i].code, &(a->code[i].term));
				}
			}else if(likely(a->code[i].iteration_skip == 0 || a->tick%(a->code[i].iteration_skip+1) == 0)){
				if(a->code[i].thread != nullptr){
					stop_infinite_loop(&(a->code[i]));
				}
				a->code[i].code();
			}
		}
	}
	for(uint_ i = 0;i < a->code.size();i++){
		if(a->code[i].get_settings(LOOP_CODE_PARTIAL_MT)){
			a->code[i].thread->join();
			delete a->code[i].thread;
			a->code[i].thread = nullptr;
		}
	}
	const long double end_time = get_time();
	const long double current_rate = 1/(end_time-start_time);
	if(unlikely(a->average_rate == 0)){
		a->average_rate = current_rate;
	}
	a->average_rate += current_rate;
	a->average_rate *= .5;
	if((a->settings & LOOP_PRINT_THIS_TIME) != 0){
		summary += "current frame rate: " + std::to_string(current_rate) + "\naverage framerate: " + std::to_string(a->average_rate) + "\nloop_settings: " + std::to_string(a->settings) + "\n";
		summary += "\tTitle\tIteration Skip\n";
		for(uint_ i = 0;i < a->code.size();i++){
			summary += "\t" + a->code[i].name + "\t" + std::to_string(a->code[i].iteration_skip) + "\n";
		}
		printf("%s", summary.c_str());
		a->settings &= ~LOOP_PRINT_THIS_TIME;
	}
	++a->tick;
	if(check_for_parameter("--debug", argc_, argv_)) ms_sleep(1000);
}

void loop_del(loop_t *a, void(*b)()){
	const uint_ code_size = a->code.size();
	for(uint_ i = 0;i < code_size;i++){
		if(a->code[i].code == b){
			if(a->code[i].thread != nullptr){
				stop_infinite_loop(&(a->code[i]));
			}
			a->code.erase(a->code.begin()+i);
			break;
		}
	}
}

void loop_del(loop_t *a, std::string b){
	const uint_ code_size = a->code.size();
	for(uint_ i = 0;i < code_size;i++){
		if(a->code[i].name == b){
			a->code[i].term = true;
			break;
		}
	}
}

bool infinite_loop(){
	return unlikely(check_signal(SIGNAL_QUIT_LOOP) == false);
}

void blank(){}

loop_entry_t loop_generate_entry(loop_entry_t a, std::string name, void(*function)()){
	a.name = name;
	a.code = function;
	return a;
}
