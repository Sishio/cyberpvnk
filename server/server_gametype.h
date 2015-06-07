#ifndef SERVER_GAMETYPE_H
	#define SERVER_GAMETYPE_H
	#include "../class/class_array.h"
	class gametype_t{
	private:
	public:
		array_t array;
		gametype_t();
		void engine();
		~gametype_t();
	};
	class gametype_info_t{
	private:
	public:
		array_t array;
		array_id_t gametype_id;
		gametype_info_t();
		~gametype_info_t();
	};
	extern void gametype_init();
	extern void gametype_close();
#endif
