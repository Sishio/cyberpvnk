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
extern void console_init();
extern void console_engine();
extern void console_close();
extern void function_add(std::pair<void(*)(), std::string>, void*);
extern void function_del(void*, void*); // function & vector
extern void* function_get(std::string a, void*); // name & vectorcr
#endif
