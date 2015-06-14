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

static void infinite_loop_function(void(*code)(), int_ *term_pointer){
	while(likely(infinite_loop() && *term_pointer == false)){
		code();
	}
}

loop_entry_t::loop_entry_t() : array(this, "loop_entry_t", ARRAY_SETTING_IMMUNITY){
	array.add_int(std::make_pair(&iteration_skip, "iteration skip"));
	iteration_skip = 0;
	array.add_void_ptr(std::make_pair((void**)&code, "code"));
	code = nullptr;
	array.add_int(std::make_pair(&term, "term"));
	term = false;
	array.add_int(std::make_pair(&settings, "settings"));
	settings = 0;
	array.add_void_ptr(std::make_pair((void**)&thread, "thread (neverend)"));
	thread = nullptr;
	array.add_long_double(std::make_pair(&start_time, "start time"));
	start_time = 0;
	array.add_long_double(std::make_pair(&end_time, "end time"));
	end_time = 0;
}

bool loop_entry_t::get_settings(int_ settings_){
	return (settings & settings_) == settings_;
}

void loop_entry_t::set_settings(int_ settings_){
	settings |= settings_;
}
void loop_add(loop_t *a, loop_entry_t *b){
	a->code.push_back(b->array.id);
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
	if(check_for_parameter("--no-mt", argc_, argv_)){
		a->settings &= ~LOOP_CODE_NEVEREND_MT;
		a->settings &= ~LOOP_CODE_PARTIAL_MT;
	}
	std::string summary = a->array.name + "\n";
	const uint_ code_size = a->code.size();
	const long double start_time = get_time();
	for(uint_ i = 0;i < a->code.size();i++){
		try{
			loop_entry_t *tmp_entry = (loop_entry_t*)find_pointer(a->code[i]);
			throw_if_nullptr(tmp_entry);
			if(check_for_parameter("--no-mt", argc_, argv_)){
				tmp_entry->set_settings(0);
				tmp_entry->set_settings(0);
			}
			tmp_entry->array.data_lock.lock();
			if(tmp_entry->term == true){
				if(tmp_entry->thread != nullptr){
					stop_infinite_loop(tmp_entry);
				}else{
					a->code.erase(a->code.begin()+i);
				}
			}else if(tmp_entry->code != nullptr){ // Deleting its place in the array allows for a seg fault if both pieces run
				if(tmp_entry->get_settings(LOOP_CODE_PARTIAL_MT)){
					//printf_("DEBUG: loop_run: Running the following loop_entry_t in its own thread (LOOP_CODE_PARTIAL_MT):" + tmp_entry->array.print() + "\n", PRINTF_DEBUG);
					tmp_entry->thread = new std::thread(tmp_entry->code);
					tmp_entry->start_time = get_time();
				}else if(tmp_entry->get_settings(LOOP_CODE_NEVEREND_MT)){
					if(tmp_entry->thread == nullptr){
						//printf_("DEBUG: loop_run: Running the following loop_entry_t in its own thread (LOOP_CODE_NEVEREND_MT):" + tmp_entry->array.print() + "\n", PRINTF_DEBUG);
						tmp_entry->thread = new std::thread(infinite_loop_function, tmp_entry->code, &(tmp_entry->term));
						tmp_entry->start_time = get_time();
					}
				}else if(likely(tmp_entry->iteration_skip == 0 || a->tick%(tmp_entry->iteration_skip+1) == 0)){
					if(tmp_entry->thread != nullptr){
						stop_infinite_loop(tmp_entry);
					}
					tmp_entry->start_time = get_time();
					//printf_("DEBUG: loop_run: Running the following loop_entry_t in the current thread:" + tmp_entry->array.print() + "\n", PRINTF_DEBUG);
					tmp_entry->code();
				}
			}
			tmp_entry->array.data_lock.unlock();
			/*
			  WARNING: be careful of throwing something that misses a vital unlock
			 */
		}catch(std::logic_error &e){}
	}
	for(uint_ i = 0;i < a->code.size();i++){
		try{
			loop_entry_t *tmp_entry = (loop_entry_t*)find_pointer(a->code[i]);
			throw_if_nullptr(tmp_entry);
			tmp_entry->array.data_lock.lock();
			if(tmp_entry->get_settings(LOOP_CODE_PARTIAL_MT)){
				tmp_entry->thread->join();
				tmp_entry->end_time = get_time();
				delete tmp_entry->thread;
				tmp_entry->thread = nullptr;
			}
			tmp_entry->array.data_lock.unlock();
		}catch(std::logic_error &e){}
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
			try{
				loop_entry_t *tmp = (loop_entry_t*)find_pointer(a->code[i]);
				summary += "\t" + tmp->name + "\t" + std::to_string(tmp->iteration_skip) + "\n";
			}catch(std::logic_error &e){}
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
		try{
			loop_entry_t *tmp = (loop_entry_t*)find_pointer(a->code[i]);
			throw_if_nullptr(tmp);
			tmp->array.data_lock.lock();
			if(tmp->code == b){
				if(tmp->thread != nullptr){
					stop_infinite_loop(tmp);
				}
				a->code.erase(a->code.begin()+i);
				tmp->array.data_lock.unlock();
				return;
			}
			tmp->array.data_lock.unlock();
		}catch(std::logic_error &e){}
	}
}

void loop_del(loop_t *a, std::string b){
	const uint_ code_size = a->code.size();
	for(uint_ i = 0;i < code_size;i++){
		try{
			loop_entry_t *tmp = (loop_entry_t*)find_pointer(a->code[i]);
			throw_if_nullptr(tmp);
			tmp->array.data_lock.lock();
			if(tmp->name == b){
				tmp->term = true;
				tmp->array.data_lock.unlock();
				return;
			}
			tmp->array.data_lock.unlock();
		}catch(std::logic_error &e){}
	}
}

bool infinite_loop(){
	return unlikely(check_signal(SIGNAL_QUIT_LOOP) == false);
}

void blank(){}

loop_entry_t* loop_generate_entry(int_ settings, std::string name, void(*function)()){
	loop_entry_t *a = new loop_entry_t();
	a->name = name;
	a->code = function;
	a->settings = settings;
	return a;
}
