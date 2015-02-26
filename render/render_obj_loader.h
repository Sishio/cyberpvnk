//http://www.youtube.com/user/thecplusplusguy
//Free source, modify if you want, LGPL licence (I guess), I would be happy, if you would not delete the link
//so other people can see the tutorial
//this file is the objloader.h
#ifndef RENDER_OBJ_LOADER_H
	#define RENDER_OBJ_LOADER_H
	#include "render_main.h"
	#include "../class/class_main.h"
	#include <SDL2/SDL.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <cstdlib>
	#include <vector>
	#include <string>
	#include <algorithm>
	#include <fstream>
	#include <cstdio>
	#include <iostream>

	class objloader_t{
	private:
		std::vector<std::string*> coord;	//every line of code from the obj file
		std::vector<coordinate*> vertex;	//all vertexes
		std::vector<face*> faces;					//all faces
		std::vector<coordinate*> normals;	//all normal vectors
		std::vector<unsigned int> texture; //the id for all the textures (so we can delete the textures after use it)
		std::vector<unsigned int> lists;	//the id for all lists (so we can delete the lists after use it)
		std::vector<material*> materials;	//all materials
		std::vector<texcoord*> texturecoordinate;	//all texture coorinate (UV coordinate)
		bool ismaterial,isnormals,istexture;	//obvious
		unsigned int loadTexture(std::string image_path);	//private load texture function
		void clean();	//free all of the used memory
	public:
		objloader_t();	
		~objloader_t();	//free the textures and lists
		int load(model_t *model);	//the main model load function
	};
	void model_load(model_t* a, std::string file = "");
#endif
