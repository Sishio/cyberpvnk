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
		std::vector<std::vector<double> > v;     // vector
		std::vector<std::vector<double> > vt;    // vector texture
		std::vector<std::vector<double> > vn;    // vector normal
		std::vector<std::vector<double> > vp;
		std::vector<std::vector<std::vector<int> > > f;  // faces
		model_t();
		void load(std::string);
		void get_size(long double*, long double*, long double*);
		void close();
	};
	class coord_t{
	public:
		array_t *array;
		long double x,y,z,x_angle,y_angle;
		long double x_vel,y_vel,z_vel;
		long double physics_time;
		long double old_time;
		bool mobile;
		coord_t();
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
		coord_t *coord;
		model_t *model;
		net_ip_connection_info_t net_ip_connection_info;
		client_t();
		void close();
	};
#endif
