/*
Czech_mate by Daniel
This file is part of Czech_mate.

Czech_mate is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as published by
the Free Software Foundation, 

Czech_mate is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Czech_mate.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CLASS_MAIN_H
	#define INPUT_MAP_SIZE 512
	#include "class_array.h"
	#include "class_extra.h"
	#include "../util/util_main.h"
	#include "sstream"
	#include "algorithm"
	#include "vector"
	#include "string"
	#include "climits"
	#include "cstdio"
	#define CLASS_MAIN_H
	#define CLASS_DATA_UPDATE_EVERYTHING	UINT_MAX
	#define CLASS_DATA_COORD_BIT		0
	#define CLASS_DATA_MODEL_BIT		1
	#define CLASS_DATA_RENDER_BUFFER_BIT	2
	#define CLASS_DATA_INPUT_BUFFER_BIT	3
	#define CLASS_DATA_NET_IP_CONNECTION_INFO_BIT	4
	#define CLASS_DATA_CLIENT_BIT		5
	#define COORD_POINTER_SIZE 1024
	#define MODEL_POINTER_SIZE 1024
	#define INPUT_TYPE_KEYBOARD 0
		#define INPUT_TYPE_KEYBOARD_INT_SIZE 2
		#define INPUT_TYPE_KEYBOARD_KEY 0 // SDLK_*
		#define INPUT_TYPE_KEYBOARD_CHAR 1
	#define INPUT_TYPE_MOUSE_MOTION 1
		#define INPUT_TYPE_MOUSE_MOTION_INT_SIZE 2
		#define INPUT_TYPE_MOUSE_MOTION_X 0
		#define INPUT_TYPE_MOUSE_MOTION_Y 1
	#define INPUT_TYPE_MOUSE_PRESS 2 // type
		#define INPUT_TYPE_MOUSE_PRESS_INT_SIZE 1
		#define INPUT_TYPE_MOUSE_PRESS_BUTTON 0; // ID entry
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_LEFT 0 // values
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_MIDDLE 1
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_RIGHT 2
	#define INPUT_TYPE_MOUSE_SCROLL 3
		#define INPUT_TYPE_MOUSE_SCROLL_UP 0
		#define INPUT_TYPE_MOUSE_SCROLL_DOWN 1
	#define INPUT_MOTION_FORWARD 0
	#define INPUT_MOTION_BACKWARD 1
	#define INPUT_MOTION_LEFT 2
	#define INPUT_MOTION_RIGHT 3
	#define GAMETYPE_FFA 1
	#define GAMETYPE_TDM 2
	#define GAMETYPE_FREE 3
	#define GAMETYPE_WIN_POINT_COUNT 1
	#define GAMETYPE_POINT_KILL 1
	/*
	Only the client and server classes are here
	gametype_t and all of the other server-centric classes are
	declared in their corresponding server file
	*/
	class input_keyboard_map_t{
	public:
		array_id_t client_id;
		array_t array;
		int keyboard_map[INPUT_MAP_SIZE];
		input_keyboard_map_t();
		~input_keyboard_map_t();
	};
	class net_ip_connection_info_t{
	public:
		array_t array;
		int connection_type; // UDP or TCP
		std::string ip;
		int port;
		net_ip_connection_info_t(bool add_to_array_vector = true);
		void set(std::string, int, int);
		~net_ip_connection_info_t();
	};
	class input_settings_t{
	public:
		input_settings_t(bool add_to_array_vector = true);
		~input_settings_t();
		array_t array;
		int int_data[64][2];
	};
	struct face{
		int facenum;
		bool four;
		int faces[4];
		int texcoord[4];
		int mat;
		face(int facen, int f1, int f2, int f3, int f4, int t2, int t3, int m);
		face(int facen, int f1, int f2, int f3, int f4, int t1, int t2, int t3, int t4, int m);
	}; 
	struct coordinate{
		float x,y,z;
		coordinate(float a,float b,float c);
	};
	struct material{
		std::string name;
		float alpha,ns,ni;
		float dif[3],amb[3],spec[3];
		int illum;
		int texture;
		material(const char*  na,float al,float n,float ni2,float* d,float* a,float* s,int i,int t);
	};
	struct texcoord{
		float u,v;
		texcoord(float a,float b);
	};
	struct pointer_device_t{
		pointer_device_t(void* id):pointer(id){
		}
		bool operator()(void *test){
			return test == pointer;
		}
		void *pointer;
	};
	class model_t{
	private:
		void load_parse_vector(std::string);
	public:
		std::string entire_object_file; // useful since we cannot network vectors yet
		std::vector<std::string> entire_material_file;
		array_t array;
		std::vector<std::string*> coord;
		std::vector<coordinate*> vertex;
		std::vector<face*> faces;
		std::vector<coordinate*> normals;
		std::vector<unsigned int> texture;
		std::vector<unsigned int> lists;
		std::vector<material*> materials;
		std::vector<texcoord*> texturecoordinate;
		bool ismaterial,isnormals,istexture;
		model_t(bool add_to_array_vector = true);
		~model_t();
		void get_size(long double*, long double*, long double*);
		void close();
		unsigned long long int collective_size;
		void update_array();
	};
	class coord_t{
	public:
		array_t array;
		long double x,y,z,x_angle,y_angle;
		long double x_vel,y_vel,z_vel,x_angle_vel,y_angle_vel;
		long double physics_time;
		long double old_time;
		bool mobile;
		coord_t(bool add_to_array_vector = true);
		void update_array_pointers();
		void print();
		void set_x_angle(bool,long double);
		void set_y_angle(bool,long double);
		~coord_t();
		array_id_t model_id;
	};
	class client_t{
	public:
		array_t array;
		array_id_t coord_id;
		array_id_t model_id;
		array_id_t connection_info_id;
		array_id_t keyboard_map_id;
		client_t(bool add_to_array_vector = true);
		~client_t();
		void update_array();
	};
#endif
