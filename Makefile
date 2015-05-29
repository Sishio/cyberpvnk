CC=clang++
ADD=-g -fno-omit-frame-pointer
CFLAGS=-std=c++11 -pipe -Wall -Wextra -Wpedantic -Wno-error=unused-parameter $(ADD)
LINKER=-lSDL2 -lm -lGL -lSDL2_net -pthread -lSDL2_image

all: client server

server_delete:
	touch bin/server.$(shell uname -m)
	rm -r bin/server.$(shell uname -m)

client_delete:
	touch bin/client.$(shell uname -m)
	rm -r bin/client.$(shell uname -m)

server_main:
	$(CC) -c $(CFLAGS) server/server_main.cpp -o server/obj/server_main.o
server_physics:
	$(CC) -c $(CFLAGS) server/server_physics.cpp -o server/obj/server_physics.o
server_net:
	$(CC) -c $(CFLAGS) server/server_net.cpp -o server/obj/server_net.o
server_console:
	$(CC) -c $(CFLAGS) server/server_console.cpp -o server/obj/server_console.o
server_input:
	$(CC) -c $(CFLAGS) server/server_input.cpp -o server/obj/server_input.o
server_gametype:
	$(CC) -c $(CFLAGS) server/server_gametype.cpp -o server/obj/server_gametype.o

server: server_gametype server_delete server_main server_input server_physics server_net server_console class net input math util loop server_delete
	ld -r server/obj/server_gametype.o server/obj/server_input.o server/obj/server_console.o server/obj/server_main.o server/obj/server_physics.o server/obj/server_net.o -o server/obj/server.o
	$(CC) $(CFLAGS) loop/obj/loop.o input/obj/input.o math/obj/math.o class/obj/class.o net/obj/net.o util/obj/util.o server/obj/server.o -o bin/server.$(shell uname -m) $(LINKER)

loop:
	$(CC) -c $(CFLAGS) loop/loop_main.cpp -o loop/obj/loop.o

c_input_engine:
	$(CC) -c $(CFLAGS) client/c_input_engine.cpp -o client/obj/c_input_engine.o

c_net_engine:
	$(CC) -c $(CFLAGS) client/c_net_engine.cpp -o client/obj/c_net_engine.o

c_render_engine:
	$(CC) -c $(CFLAGS) client/c_render_engine.cpp -o client/obj/c_render_engine.o

c_test_logic:
	$(CC) -c $(CFLAGS) client/c_test_logic.cpp -o client/obj/c_test_logic.o

c_engine: c_input_engine c_net_engine c_render_engine c_test_logic
	$(CC) -c $(CFLAGS) client/c_menu_engine.cpp -o client/obj/c_menu_engine.o
	ld -r client/obj/c_test_logic.o client/obj/c_net_engine.o client/obj/c_menu_engine.o client/obj/c_render_engine.o client/obj/c_input_engine.o -o client/obj/c_engine.o


render:
	$(CC) -c $(CFLAGS) render/render_main.cpp -o render/obj/render.o

client: render loop client_delete class net input util math c_engine
	$(CC) -c $(CFLAGS) client/c_main.cpp -o client/obj/c_main.o
	ld -r client/obj/c_main.o client/obj/c_engine.o -o client/obj/c.o
	$(CC) $(CFLAGS) util/obj/util.o math/obj/math.o render/obj/render.o class/obj/class.o net/obj/net.o input/obj/input.o loop/obj/loop.o client/obj/c.o -o bin/client.$(shell uname -m) $(LINKER)

console: util net
	$(CC) $(CFLAGS) net/obj/net.o console/console_main.cpp util/obj/util_main.o -o bin/console.$(shell uname -m) $(LINKER)

class: loop util
	$(CC) -c $(CFLAGS) class/class_main.cpp -o class/obj/class_main.o
	$(CC) -c $(CFLAGS) class/class_array.cpp -o class/obj/class_array.o
	$(CC) -c $(CFLAGS) class/class_extra.cpp -o class/obj/class_extra.o
	ld -r class/obj/class_main.o class/obj/class_array.o class/obj/class_extra.o -o class/obj/class.o

input: loop util
	$(CC) -c $(CFLAGS) input/input_main.cpp -o input/obj/input.o

net: loop util
	$(CC) -c $(CFLAGS) net/net_main.cpp -o net/obj/net_main.o
	$(CC) -c $(CFLAGS) net/net_ip.cpp -o net/obj/net_ip.o
	$(CC) -c $(CFLAGS) net/net_store.cpp -o net/obj/net_store.o
	ld -r net/obj/net_main.o net/obj/net_ip.o net/obj/net_store.o -o net/obj/net.o

util:
	$(CC) -c $(CFLAGS) util/util_main.cpp -o util/obj/util.o

math:
	$(CC) -c $(CFLAGS) math/math_main.cpp -o math/obj/math.o
