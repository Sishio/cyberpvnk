#include "../main.h" // debug macros
#define NET_CLIENT_PORT 50001
#define NET_SERVER_PORT 50000
#define NET_MTU 484 // The absolute highest data MTU possible (the rest would be taken up at the hardware level)
#define NET_MTU_OVERHEAD 40 
#define NET_COORD 0
#define NET_MODEL 1
// class_array.h gets 1-0e
// net_const.h (this) gets 17-1d
// net_ip.h gets 1e-1f
typedef unsigned long int net_packet_id;
#ifdef DEBUG_SYMBOLS
	#define NET_PACKET_ID_START	(char*)"X"
	#define NET_PACKET_ID_END	(char*)"W"
	#define NET_PACKET_POS_START	(char*)"V"
	#define NET_PACKET_POS_END	(char*)"U"
	#define NET_PACKET_START	(char*)"T"
	#define NET_PACKET_END		(char*)"S"
	#define NET_JOIN		(char*)"R"
#elif ALPHA_NEGATIVE_SEPERATOR
	#define NET_PACKET_ID_START	-23
	#define NET_PACKET_ID_END	-24
	#define NET_PACKET_POS_START	-25
	#define NET_PACKET_POS_END	-26
	#define NET_PACKET_START	-27
	#define NET_PACKET_END		-28
	#define NET_JOIN		-29
#else
	#define NET_PACKET_ID_START	(char*)"\x17"
	#define NET_PACKET_ID_END	(char*)"\x18"
	#define NET_PACKET_POS_START	(char*)"\x19"
	#define NET_PACKET_POS_END	(char*)"\x1a"
	#define NET_PACKET_START	(char*)"\x1b"
	#define NET_PACKET_END		(char*)"\x1c"
	#define NET_JOIN		(char*)"\x1d"
#endif
