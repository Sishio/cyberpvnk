#include "c_main.h"
#include "c_test_logic.h"

bool test_init = false;

void test_logic_init(){
	loop_add(loop, loop_generate_entry(loop_entry_t(), "test_logic_loop", test_logic_loop));
	test_init = true;
}

void test_logic_loop(){
	if(unlikely(test_init == false)){
		test_logic_init();
	}
}

void test_logic_close(){
	loop_del(loop, test_logic_loop);
}
