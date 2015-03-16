#ifndef SERVER_PRINT_H
	#define SERVER_PRINT_H
	#include "cstdio"
	#include "vector"
	#define PRINT_LEVEL_URGENT	0
	#define PRINT_LEVEL_WARN	1
	#define PRINT_LEVEL_NOTICE	2
	class output_t{
	public:
		output_t();
		~output_t();
		std::string data;
		int print_level;
	};
	extern std::vector<output_t> output;
#endif
