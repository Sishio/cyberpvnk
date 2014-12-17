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
	#include "class_array.h"
	#include "class_extra.h"
	#include "../net/net_ip.h"
	#include "../net/net_serial.h"
	#include "sstream"
	#include "vector"
	#define COORD_POINTER_SIZE 1024
	#define MODEL_POINTER_SIZE 1024
	class model_t{
	private:
		void load_parse_vector(std::string);
	public:
		array_t *array;
		model_t *pointer_to_self;
		std::vector<std::vector<long double> > v;     // vector
		std::vector<std::vector<long double> > vt;    // vector texture
		std::vector<std::vector<long double> > vn;    // vector normal
		std::vector<std::vector<long double> > vp;
		std::vector<std::vector<std::vector<int> > > f;  // faces
		model_t();
		void load(std::string);
		void get_size(long double*, long double*, long double*);
		void close();
		unsigned long long int collective_size;
		void update_array_pointers();
	};
	class coord_t{
	public:
		array_t *array;
		coord_t *pointer_to_self; // this is needed for the reconstructors
		long double x,y,z,x_angle,y_angle;
		long double x_vel,y_vel,z_vel;
		long double physics_time;
		long double old_time;
		bool mobile;
		coord_t();
		void update_array_pointers();
		void print();
		void set_x_angle(bool,long double);
		void set_y_angle(bool,long double);
		void close();
		std::vector<unsigned long int> nearby_coord;
		int model_id;
	};
	class client_t{
	public:
		array_t *array;
		client_t *pointer_to_self;
		int coord_id;
		int model_id;
		net_ip_connection_info_t net_ip_connection_info;
		client_t();
		void update_array();
		void close();
	};
	class input_buffer_t;
	#include "../input/input_main.h"
	class client_extra_t{
	public:
		std::vector<input_buffer_t> input_buffer;
		client_extra_t();
	};
	extern std::vector<client_t*> client_vector;
	extern std::vector<coord_t*> coord_vector;
	extern std::vector<model_t*> model_vector;
	extern void add_coord(coord_t*);
	extern void add_model(model_t*);
	extern coord_t *find_coord_pointer(int);
	extern model_t *find_model_pointer(int);
#endif
