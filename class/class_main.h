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
		void get_size(double*,double*,double*);
		void close();
	};
	class coord_t{
	public:
		array_t *array;
		double x,y,z,x_angle,y_angle;
		double x_vel,y_vel,z_vel;
		double physics_time;
		double old_time;
		bool mobile;
		void init();
		void print();
		void set_x_angle(bool,double);
		void set_y_angle(bool,double);
		void close();
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
