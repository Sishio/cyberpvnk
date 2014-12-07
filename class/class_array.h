#ifndef CLASS_ARRAY_H
	#define CLASS_ARRAY_H
	#include "../util/util_main.h"
	#include "cstdio"
	#include "string"
	#include "cstdio"
	#include "cstdlib"
	#include "vector"
	#define ARRAY_ITEM_SEPARATOR_START	'\02'
	#define ARRAY_ITEM_SEPARATOR_END	'\03'
	#define ARRAY_TYPE_SEPARATOR_START	'\01'
	#define ARRAY_TYPE_SEPARATOR_END	'\04'
	#define ARRAY_HEADER_COORD_T	1 // 0 cannot be used since if no data is received, data[0] == NULL == 0
	#define ARRAY_HEADER_MODEL_T	2
	#define ARRAY_HEADER_CLIENT_T	3
	#define ID_COUNT 4
	class array_t{
	public:
		int header;
		unsigned long int id;
		std::vector<int*> int_array;
		std::vector<double*> double_array;
		std::vector<std::string*> string_array;
		array_t(int);
		bool id_match(unsigned long int);
		void set_header(int);
		std::string gen_string(); // generates the string without any seperators
		void parse_string(std::string);
		void close();
	};
#endif
