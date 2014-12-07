CC=clang++
MACROS=
CFLAGS=-std=c++11 -Wall -Werror -Wextra -g -Wno-error=unused-parameter $(MACROS)
LINKER=-lSDL2 -lm -lGL -lSDL2_net -pthread

server:
	make -B class
	make -B net
	make -B util
	make -B math
	make -B thread
	$(CC) -c $(CFLAGS) server/server_main.cpp -o server/obj/server_main.o
	$(CC) -c $(CFLAGS) server/server_physics.cpp -o server/obj/server_physics.o
	$(CC) -c $(CFLAGS) server/server_net.cpp -o server/obj/server_net.o
	$(CC) -c $(CFLAGS) server/server_console.cpp -o server/obj/server_console.o
	$(CC) -c $(CFLAGS) server/server_class.cpp -o server/obj/server_class.o
	ld -r server/obj/server_main.o server/obj/server_class.o  server/obj/server_physics.o server/obj/server_net.o server/obj/server_console.o -o server/obj/server.o
	$(CC) $(CFLAGS) math/obj/math.o class/obj/class.o net/obj/net.o thread/obj/thread.o util/obj/util.o server/obj/server.o -o bin/server.$(shell uname -m) $(LINKER)

thread:
	$(CC) -c $(CFLAGS) thread/thread_main.cpp -o thread/obj/thread.o 

console:
	make -B net
	make -B util
	$(CC) $(CFLAGS) net/obj/net.o console/console_main.cpp util/obj/util_main.o -o bin/console.$(shell uname -m) $(LINKER)

class:
	$(CC) -c $(CFLAGS) class/class_main.cpp -o class/obj/class_main.o
	$(CC) -c $(CFLAGS) class/class_array.cpp -o class/obj/class_array.o
	ld -r class/obj/class_main.o class/obj/class_array.o -o class/obj/class.o

input:
	$(CC) -c $(CFLAGS) input/input_main.cpp -o input/obj/input.o

net:
	$(CC) -c $(CFLAGS) net/net_main.cpp -o net/obj/net_main.o
	$(CC) -c $(CFLAGS) net/net_serial.cpp -o net/obj/net_serial.o
	$(CC) -c $(CFLAGS) net/net_ip.cpp -o net/obj/net_ip.o
	ld -r net/obj/net_main.o net/obj/net_serial.o net/obj/net_ip.o -o net/obj/net.o

render:
	$(CC) -c $(CFLAGS) render/render_main.cpp -o render/obj/render_main.o
	$(CC) -c $(CFLAGS) render/render_obj_loader.cpp -o render/obj/render_obj_loader.o
	ld -r render/obj/render_obj_loader.o render/obj/render_main.o -o render/obj/render.o

util:
	$(CC) -c $(CFLAGS) util/util_main.cpp -o util/obj/util.o

math:
	$(CC) -c $(CFLAGS) math/math_main.cpp -o math/obj/math.o
