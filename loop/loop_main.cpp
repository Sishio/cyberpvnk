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

static void infinite_loop_function(void(*code)(), bool *term_pointer){
	while(likely(infinite_loop() && *term_pointer == false)){
		code();
	}
}

loop_t::loop_t(){
	target_rate = 60;
	average_rate = 0;
	settings = 0;
}

loop_entry_t::loop_entry_t(){
	iteration_skip = 0;
	code = nullptr;
	term = false;
}

void loop_add(loop_t *a, std::string name, void(*b)()){
	loop_entry_t tmp;
	tmp.code = b;
	tmp.name = name;
	a->code.push_back(tmp);
	if(a->neverend_threads.size() != 0){
		a->neverend_threads.push_back(new std::thread(infinite_loop_function, b, &(tmp.term)));
	}
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

static void loop_update_neverend_thread(loop_t *a){
	if(a->neverend_threads.size() != a->code.size()){
		for(uint_ i = 0;i < a->code.size();i++){
			a->code[i].term = true;
		}
		for(uint_ i = 0;i < a->neverend_threads.size();i++){
			a->neverend_threads[i]->join();
			delete a->neverend_threads[i];
		}
		for(uint_ i = 0;i < a->code.size();i++){
			a->code[i].term = false;
			a->neverend_threads.push_back(new std::thread(infinite_loop_function, a->code[i].code, &(a->code[i].term)));
		}
	}
}

void loop_run(loop_t *a){
	const bool print_this_time = CHECK_BIT(settings, LOOP_PRINT_THIS_TIME);
	if(print_this_time){
		FLIP_BIT(settings, LOOP_PRINT_THIS_TIME);
		printf_("settings: " + std::to_string(settings) + "\n", PRINTF_VITAL);
	}
	std::string summary = a->name + "\n";
	const uint_ code_size = a->code.size();
	const long double start_time = get_time();
	if(CHECK_BIT(settings, LOOP_CODE_PARTIAL_MT) == 1){
		std::vector<std::thread*> thread;
		for(uint_ i = 0;i < code_size;i++){
			if(loop_entry_will_run(a, &a->code[i])){
				thread.push_back(new std::thread(a->code[i].code));
			}
		}
		for(uint_ i = 0;i < code_size;i++){
			thread[i]->join();
			delete thread[i];
			thread[i] = nullptr;
		}
	}else if(CHECK_BIT(settings, LOOP_CODE_NEVEREND_MT) == 0){
		for(uint_ i = 0;i < code_size;i++){
			if(loop_entry_will_run(a, &a->code[i])){
				const long double start_time = get_time();
				a->code[i].code();
				if(likely(print_this_time)){
					summary += "\t" + a->code[i].name + "\t\t" + std::to_string(get_time()-start_time) + "\n";
				}
			} // perhaps print a little notice if it isn't running?
		}
		if(unlikely(a->neverend_threads.size() != 0)){
			for(uint_ i = 0;i < a->code.size();i++){
				a->code[i].term = true;
			}
			for(uint_ i = 0;i < a->neverend_threads.size();i++){
				a->neverend_threads[i]->join();
				delete a->neverend_threads[i];
			}
			a->neverend_threads.clear();
		}
	}else if(CHECK_BIT(settings, LOOP_CODE_NEVEREND_MT) == 1){
		loop_update_neverend_thread(a);
	}
	const long double end_time = get_time();
	const long double current_rate = 1/(end_time-start_time);
	if(unlikely(a->average_rate == 0)){
		a->average_rate = current_rate;
	}
	a->average_rate += current_rate;
	a->average_rate *= .5;
	if(print_this_time){
		summary += "current frame rate: " + std::to_string(current_rate) + "\naverage framerate: " + std::to_string(a->average_rate) + "\nloop_settings: " + std::to_string(settings) + "\n";
		if(a->neverend_threads.size() != 0){
			summary += "neverend_thread is in use with " + std::to_string(a->neverend_threads.size()) + " threads.\n";
		}else{
			summary += "neverend_thread is not in use\n";
		}// unify all of this into some sort of standard
		summary += "\tTitle\tIteration Skip\n";
		for(uint_ i = 0;i < a->code.size();i++){
			summary += "\t" + a->code[i].name + "\t" + std::to_string(a->code[i].iteration_skip) + "\n";
		}
		printf("%s", summary.c_str());
	}
	++a->tick;
	if(check_for_parameter("--debug", argc_, argv_)) ms_sleep(1000);
}

void loop_del(loop_t *a, void(*b)()){
	const uint_ code_size = a->code.size();
	for(uint_ i = 0;i < code_size;i++){
		if(a->code[i].code == b){
			if(a->neverend_threads.size() != 0){
				const uint_ old_size = a->neverend_threads.size();
				a->code[i].term = true;
				while(a->neverend_threads.size() == old_size){ms_sleep(.0001);}
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
			if(a->neverend_threads.size() != 0){
				const uint_ old_size = a->neverend_threads.size();
				a->code[i].term = true;
				while(a->neverend_threads.size() == old_size){ms_sleep(.0001);}
			}
			a->code.erase(a->code.begin()+i);
			break;
		}
	}
}

bool infinite_loop(){
	return unlikely(check_signal(SIGNAL_QUIT_LOOP) == false);
}

void blank(){}

/*loop_thread_pool_t::loop_thread_pool_t(int core_count_){
	core_count = core_count_;
	std::thread a;
	if(core_count == 0){
		core_count = 1;
	}
	if(core_count > 1024){
		core_count = 1024;
	}
	for(int i = 0;i < core_count;i++){
		thread_vector[i] = std::async(blank);
	}
	//printf("loop_thread_pool_t: core_count = %d\n", core_count);
}

loop_thread_pool_t::~loop_thread_pool_t(){wait();}

int loop_thread_pool_t::find_empty_thread(){
	while(true){
		for(int i = 0;i < core_count;i++){
			if(thread_vector[i].valid()){
				if(thread_vector[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready){ // if the thread is not currently running code
					return i;
				}
			}
		}
	}
}

void loop_thread_pool_t::add(void(*code)()){
	thread_vector[find_empty_thread()] = std::async(code);
}

void loop_thread_pool_t::add(void(*code)(void*), void *a){
	thread_vector[find_empty_thread()] = std::async(code, a);
}

void loop_thread_pool_t::wait(){
	//for(int i = 0;i < core_count;i++){
		//if(thread_vector[i].valid()) thread_vector[i].get();
		//}
}
*/
