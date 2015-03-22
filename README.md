This is Czech_mate, a little FPS engine I decided to create. It is licensed under the GNU GPLv2.

There is no version history with Czech_mate since it is not far enough along yet but I will update this when the time comes. There are a few different files you will get when you download or clone the repository:

Before I tell you all what is contained in here, I need to state something. A fair chunk of the code in Czech_mate is modular. This allows for different version of different libraries to fit on multiple platforms with multiple APIs. If you are interested in building a module and if you want a git tree setup here, let me know so I can configure the branch for you to work off of.

All that being said, here is the contents of Czech_mate as you should get it:

File/Folder  | Purpose
------------- | -------------
bin | The binaries for the client, server and console
class | The classes and other data structures used in the client and in the server code.
client | The client code
console | The console code. This is a different binary than the client and the server. The purpose of this is to interface with the two without having to use threads.
input | The input code. This uses SDL2 and is rather messy. I need to rewrite this since it was all done in about 10 minutes.
math | The standard math constants and other math functions are defined here. The only math functions that are not defined here are ones that include the classes since those are only used in the server and can be found in the server_physics.cpp file
net | The networking code. This also uses SDL2 for IP networking. Serial networking is something that I would like to do but setting it up is not a main priority of mine.
render | The rendering code. This uses OpenGL and SDL, it works well.
server | The server code.
util | Nice little utilities library I decided to make. Its main uses are for it's millisecond sleep (ms_sleep(double)) and pseudo-random numbers (gen_rand(max_value)).


All of the physics are done server side. You can compile your own server if you want to. The server can use as many threads as you want.

Once development gets further ahead, I will host a server on my computer 24/7. I may need some help to get there in a timely manner. If you are interested in helping, please contact me so I can help you.
