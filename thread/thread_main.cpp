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
#include "thread_main.h"

void thread_t::blank(){
	rules.thread_count = 8192;
}

void thread_t::init(int argc,char**argv){
	blank();
	thread = nullptr;
	for(int i = 0;i < argc;i++){
		if(strncmp(argv[i],(char*)"--thread-rules-count",strlen(argv[i])) == 0){
			rules.thread_count = atoi((char*)argv[i+1]);
		}
	}
	if(rules.thread_count > 0){
		thread = new SDL_Thread*[rules.thread_count];
		for(int i = 0;i < rules.thread_count;i++){
			thread[i] = nullptr;
		}
	}
}

int thread_t::run(SDL_ThreadFunction thread_to_create,void* arg){
	int return_value = -1;
	bool main_thread = true;
	for(int i = 0;i < rules.thread_count;i++){
		if(thread[i] == nullptr){
			thread[i] = SDL_CreateThread((SDL_ThreadFunction)thread_to_create,"CreateThread_thread",arg);
			return_value = i;
			main_thread = false;
		}
	}
	term_if_true(main_thread, (char *)"ran out of threads and I am to lazy to fix this right now\n");
	return return_value;
}

void thread_t::wait(int a){
	SDL_WaitThread(thread[a], nullptr);
}

void thread_t::close(){
	// try to stop the threads automatically
	for(int i = 0;i < rules.thread_count;i++){
		thread[i] = nullptr;
	}
	delete[] thread;
	thread = nullptr;
}

/*int thread_t::wait_thread(int wait_thread){
	int return_value = 0;
	#ifdef USE_SDL_THREADS
		SDL_WaitThread(thread[wait_thread],&return_value);
	#elif __linux
		pthread_join(thread[wait_thread],&return_value);
	#endif
	return (int)return_value;
}*/
