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
	#define CLASS_MAIN_H
	#include "../main.h"
	#include "class_array.h"
	#include "class_extra.h"
	#include "../util/util_main.h"
	#include "sstream"
	#include "algorithm"
	#include "vector"
	#include "string"
	#include "climits"
	#include "cstdio"
	#define CLASS_DATA_UPDATE_EVERYTHING	UINT_MAX
	#define CLASS_DATA_COORD_BIT		0
	#define CLASS_DATA_MODEL_BIT		1
	#define CLASS_DATA_RENDER_BUFFER_BIT	2
	#define CLASS_DATA_INPUT_BUFFER_BIT	3
	#define CLASS_DATA_NET_IP_CONNECTION_INFO_BIT	4
	#define CLASS_DATA_CLIENT_BIT		5
	#define COORD_POINTER_SIZE 1024
	#define MODEL_POINTER_SIZE 1024
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
	class net_ip_connection_info_t{
	public:
		array_t array;
		int_ connection_type; // UDP or TCP
		std::string ip;
		int_ port;
		net_ip_connection_info_t(bool add_to_array_vector = true);
		~net_ip_connection_info_t();
	};
	class input_settings_t{
	public:
		input_settings_t(bool add_to_array_vector = true);
		~input_settings_t();
		array_t array;
		int_ int_data[64][2];
	};
	struct face{
		int_ facenum;
		bool four;
		int_ faces[4];
		int_ texcoord[4];
		int_ mat;
		face(int_ facen, int_ f1, int_ f2, int_ f3, int_ f4, int_ t2, int_ t3, int_ m);
		face(int_ facen, int_ f1, int_ f2, int_ f3, int_ f4, int_ t1, int_ t2, int_ t3, int_ t4, int_ m);
	}; 
	struct coordinate{
		float x,y,z;
		coordinate(float a,float b,float c);
	};
	struct material{
		std::string name;
		float alpha,ns,ni;
		float dif[3],amb[3],spec[3];
		int_ illum;
		int_ texture;
		material(const char*  na,float al,float n,float ni2,float* d,float* a,float* s,int_ i,int_ t);
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
		std::vector<uint_> texture;
		std::vector<uint_> lists;
		std::vector<material*> materials;
		std::vector<texcoord*> texturecoordinate;
		bool ismaterial,isnormals,istexture;
		model_t(bool add_to_array_vector = true);
		~model_t();
		void get_size(long double*, long double*, long double*);
		void close();
		uint_ collective_size;
	};
	class coord_t{
	public:
		array_t array;
		int_ dimensions_;
		int_ dimensions();
		long double x,y,z,x_angle,y_angle;
		long double x_vel,y_vel,z_vel,x_angle_vel,y_angle_vel;
		long double physics_time;
		long double old_time;
		coord_t(bool add_to_array_vector = true);
		void print();
		void set_x_angle(bool,long double); // complex func. in .cpp
		void set_y_angle(bool,long double);
		~coord_t();
		array_id_t model_id, tile_id;
		int_ settings;
		void set_movable(bool a){SET_BIT(settings, 1, a);}
		bool get_movable(){return CHECK_BIT(settings, 1);}
		bool get_interactable(){return CHECK_BIT(settings, 0);}
		void set_interactable(bool a){SET_BIT(settings, 0, a);}
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
	};
#endif
