#ifndef SERVER_CONSOLE_H
#define SERVER_CONSOLE_H
#include "cstdio"
#include "cstdlib"
#include "cstring"
#include "iostream"
#define FUNCTION_RETURN_VOID 1
#define FUNCTION_RETURN_INT_ 16
#define FUNCTION_PARAMETER_INT_ 2
#define FUNCTION_PARAMETER_VOID_PTR 4
#define FUNCTION_PARAMETER_VOID 8
#define FUNCTION_PARAMETER_STRING 32
#define MAX_RECURSION_COUNT 16 // this could be raised a lot

struct shell_script_t{
public:
	shell_script_t();
	~shell_script_t();
	array_t array;
	std::vector<std::string> commands; // convert this to the commands when it is used to simplify editing
	std::string name;
	int_ run();
};


struct function_t{
public:
	std::string name;
	void* function;
	array_t array;
	function_t(void*, std::string, int_);
	~function_t();
	bool functions_equal(void *a){return a == function;}
	std::string get_name();
	int_ run();
	std::string return_value;
	int_ settings;
	int_ int_value[8];
	void * void_ptr_value[8];
	std::string string_value[8];
};

extern void console_init();
extern void console_engine();
extern void console_close();
extern void function_add(std::pair<void(*)(), std::string>, void*);
extern void function_del(void*, void*); // function & vector
extern void* function_get(std::string a, void*); // name & vectorcr
#endif
