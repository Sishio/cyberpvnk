#ifndef CLASS_MAIN_H
	#define CLASS_MAIN_H
	#include "class_array.h"
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
		void init();
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
		void init();
		void print();
		void set_x_angle(bool,long double);
		void set_y_angle(bool,long double);
		void close();
		std::vector<unsigned long int> nearby_coord;
		model_t *model;
	};
	class net_data_t{
	public:
		std::string ip;
		unsigned short int port;
		void init();
	};
	class client_t{
	public:
		array_t *array;
		coord_t *coord;
		model_t *model;
		net_data_t net;
		void init();
		void close();
	};
#endif
