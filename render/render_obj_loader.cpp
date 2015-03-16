// I need to convert this to OpenGL 3 since I want nice graphics without having to deal with the old OpenGL API
//This example program is created by thecplusplusuy for demonstration purposes. It's a simple 3D model loader (wavefront (.obj)), which is capable to load materials and UV textures:
//http://www.youtube.com/user/thecplusplusguy
//Free source, modify if you want, LGPL licence (I guess), I would be happy, if you would not delete the link
//so other people can see the tutorial
//this file is the objloader.cpp
#include "render_obj_loader.h"
//nothing to explain here
coordinate::coordinate(float a,float b,float c){
	x=a;
	y=b;
	z=c;
}
//nothing to explain here
face::face(int facen,int f1,int f2,int f3,int t1,int t2,int t3,int m){
	facenum=facen;
	faces[0]=f1;
	faces[1]=f2;
	faces[2]=f3;
	texcoord[0]=t1;
	texcoord[1]=t2;
	texcoord[2]=t3;
	mat=m;
	four=false;
}
//nothing to explain here
face::face(int facen,int f1,int f2,int f3,int f4,int t1,int t2,int t3,int t4,int m){
	facenum=facen;
	faces[0]=f1;
	faces[1]=f2;
	faces[2]=f3;
	faces[3]=f4;
	texcoord[0]=t1;
	texcoord[1]=t2;
	texcoord[2]=t3;
	texcoord[3]=t4;
	mat=m;
	four=true;
}
//nothing to explain here
material::material(const char*  na,float al,float n,float ni2,float* d,float* a,float* s,int i,int t){
	name=na;
	alpha=al;
	ni=ni2;
	ns=n;
	dif[0]=d[0];
	dif[1]=d[1];
	dif[2]=d[2];
	amb[0]=a[0];
	amb[1]=a[1];
	amb[2]=a[2];
	spec[0]=s[0];
	spec[1]=s[1];
	spec[2]=s[2];
	illum=i;
	texture=t;
}
//nothing to explain here
texcoord::texcoord(float a,float b){
	u=a;
	v=b;
}

static std::vector<std::string*> parse_model_file(model_t *model){
	std::vector<std::string*> tmp;
	std::string tmp_string = model->entire_object_file;
	while(tmp_string.size() > 1){
		tmp.push_back(new std::string(tmp_string.substr(0, tmp_string.find_first_of("\n"))));
		tmp_string = tmp_string.substr(tmp_string.find_first_of("\n")+1, tmp_string.size());
		printf("tmp[%d]: %s\n", tmp.size()-1, tmp[tmp.size()-1]->c_str());
		printf("remaining string to process: '%s'\n", tmp_string.c_str());
	}
	return tmp;
}
static std::vector<std::string> parse_material_file(model_t *model, int current_one){
	std::vector<std::string> tmp;
	std::string tmp_string = model->entire_material_file[current_one];
	while(tmp_string.size() > 1){
		tmp.push_back((tmp_string.substr(0, tmp_string.find_first_of("\n"))));
		tmp_string = tmp_string.substr(tmp_string.find_first_of("\n")+1, tmp_string.size());
		printf("tmp[%d]: %s\n", tmp.size()-1, tmp[tmp.size()-1].c_str());
	}
	return tmp;
}

int objloader_t::load(model_t *model){
	unsigned long int material_pos = 0;
	int curmat;
	coord = parse_model_file(model);
	for(unsigned long int i=0;i<(unsigned long int)coord.size();i++){	//and then go through all line and decide what kind of line it is
		printf("Parsing '%s'\n", coord[i]->c_str());
		if((*coord[i])[0]=='#'){	//if it's a comment
			continue;	//we don't have to do anything with it
		}else if((*coord[i])[0]=='v' && (*coord[i])[1]==' '){	//if a vertex
			float tmpx,tmpy,tmpz;
			sscanf(coord[i]->c_str(),"v %f %f %f",&tmpx,&tmpy,&tmpz);	//read the 3 floats, which makes up the vertex
			vertex.push_back(new coordinate(tmpx,tmpy,tmpz));	//and put it in the vertex vector
		}else if((*coord[i])[0]=='v' && (*coord[i])[1]=='n'){	//if it's a normal vector
			float tmpx,tmpy,tmpz;
			sscanf(coord[i]->c_str(),"vn %f %f %f",&tmpx,&tmpy,&tmpz);
			normals.push_back(new coordinate(tmpx,tmpy,tmpz));
			isnormals=true;
		}else if((*coord[i])[0]=='f'){
			int a,b,c,d,e;
			if(count(coord[i]->begin(),coord[i]->end(),' ')==4){
				if(coord[i]->find("//")!=std::string::npos){
					sscanf(coord[i]->c_str(),"f %d//%d %d//%d %d//%d %d//%d",&a,&b,&c,&b,&d,&b,&e,&b);
					faces.push_back(new face(b,a,c,d,e,0,0,0,0,curmat));
				}else if(coord[i]->find("/")!=std::string::npos){
					int t[4];
					sscanf(coord[i]->c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&a,&t[0],&b,&c,&t[1],&b,&d,&t[2],&b,&e,&t[3],&b);
					faces.push_back(new face(b,a,c,d,e,t[0],t[1],t[2],t[3],curmat));
				}else{
					sscanf(coord[i]->c_str(),"f %d %d %d %d",&a,&b,&c,&d);
					faces.push_back(new face(-1,a,b,c,d,0,0,0,0,curmat));
				}
			}else{
				if(coord[i]->find("//")!=std::string::npos){
					sscanf(coord[i]->c_str(),"f %d//%d %d//%d %d//%d",&a,&b,&c,&b,&d,&b);
					faces.push_back(new face(b,a,c,d,0,0,0,curmat));
				}else if(coord[i]->find("/")!=std::string::npos){
					int t[3];
					sscanf(coord[i]->c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d",&a,&t[0],&b,&c,&t[1],&b,&d,&t[2],&b);
					faces.push_back(new face(b,a,c,d,t[0],t[1],t[2],curmat));
				}else{
					sscanf(coord[i]->c_str(),"f %d %d %d",&a,&b,&c);
					faces.push_back(new face(-1,a,b,c,0,0,0,curmat));
				}
			}
		}else if((*coord[i])[0]=='u' && (*coord[i])[1]=='s' && (*coord[i])[2]=='e'){	//use material material_name
			char tmp[200];
			sscanf(coord[i]->c_str(),"usemtl %s",tmp);	//read the name of the material to tmp
			for(unsigned long int n=0;n<(unsigned long int)materials.size();n++){	//go through all of the materials
				if(strcmp(materials[n]->name.c_str(),tmp)==0){	//and compare the tmp with the name of the material
					curmat=(int)n;	//if it's equal then set the current material to that
					break;
				}
			}
		}else if((*coord[i])[0]=='m' && (*coord[i])[1]=='t' && (*coord[i])[2]=='l' && (*coord[i])[3]=='l'){
			ismaterial=true;
			std::vector<std::string> tmp = parse_material_file(model, material_pos);
			char name[200];	//name of the material
			char filename_[200];	//filename of the texture
			float amb[3] = {0},dif[3] = {0},spec[3] = {0},alpha = 0,ns = 0,ni = 0;	//colors, shininess, and something else
			int illum = 0;
			unsigned int texture_ = 0;
			bool ismat=false;	//do we already have a material read in to these variables?
			strcpy(filename_,"\0");	//set filename to nullbyte char*acter
			const unsigned long int tmp_size = tmp.size();
			for(unsigned long int n = 0;n < tmp_size;n++){
				if(tmp[n][0]=='#')	//we don't care about comments
					continue;
				if(tmp[n][0]=='n' && tmp[n][1]=='e' && tmp[n][2]=='w'){
					if(ismat){
						if(strcmp(filename_,"\0") != 0){
							materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,(int)texture_));	//push back
							strcpy(filename_,"\0");
						}else{
							materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,-1));		//push back, but use -1 to texture
						}
					}
					ismat=false;	//we start from a fresh material
					sscanf(tmp[n].c_str(),"newmtl %s",name);	//read in the name
				}else if(tmp[n][0]=='N' && tmp[n][1]=='s'){
					sscanf(tmp[i].c_str(),"Ns %f",&ns);
					ismat=true;
				}else if(tmp[n][0]=='K' && tmp[n][1]=='a'){
					sscanf(tmp[i].c_str(),"Ka %f %f %f",&amb[0],&amb[1],&amb[2]);
					ismat=true;
				}else if(tmp[n][0]=='K' && tmp[n][1]=='d'){
					sscanf(tmp[i].c_str(),"Kd %f %f %f",&dif[0],&dif[1],&dif[2]);
					ismat=true;
				}else if(tmp[n][0]=='K' && tmp[n][1]=='s'){
					sscanf(tmp[i].c_str(),"Ks %f %f %f",&spec[0],&spec[1],&spec[2]);
					ismat=true;
				}else if(tmp[n][0]=='N' && tmp[n][1]=='i'){
					sscanf(tmp[i].c_str(),"Ni %f",&ni);
					ismat=true;
				}else if(tmp[n][0]=='d' && tmp[n][1]==' '){
					sscanf(tmp[i].c_str(),"d %f",&alpha);
					ismat=true;
				}else if(tmp[n][0]=='i' && tmp[n][1]=='l'){
					sscanf(tmp[i].c_str(),"illum %d",&illum);
					ismat=true;
				}else if(tmp[n][0]=='m' && tmp[n][1]=='a'){
					sscanf(tmp[i].c_str(),"map_Kd %s",filename_);
					texture_=loadTexture(filename_);	//read the filename, and use the loadTexture function to load it, and get the id.
					ismat=true;
				}
			}
			if(ismat){
				if(strcmp(filename_,"\0")!=0){
					materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,(int)texture_));
				}else{
						materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,-1));
				}
			}
		}else if((*coord[i])[0]=='v' && (*coord[i])[1]=='t'){
			float u,v;
			sscanf(coord[i]->c_str(),"vt %f %f",&u,&v);	//read the uv coordinate
			texturecoordinate.push_back(new texcoord(u,1-v));	//I push back 1-v instead of normal v, because obj file use the upper left corner as 0,0 coorinate
			//but OpenGL use bottom left corner as 0,0, so I convert it
			istexture=true;
		}
	}
	if(materials.size() == 0){	//if some reason the material file doesn't contain any material, we don't have material
		ismaterial=false;
	}else{
		ismaterial=true;
	}
	std::cout << vertex.size() << " " << normals.size() << " " << faces.size() << " " << materials.size() << std::endl; 	//test purposes
	model->coord = coord;
	model->vertex = vertex;
	model->faces = faces;
	model->normals = normals;
	model->texture = texture;
	model->lists = lists;
	model->materials = materials;
	model->texturecoordinate = texturecoordinate;
	model->ismaterial = ismaterial;
	model->isnormals = isnormals;
	model->istexture = istexture;
	clean();
	return 0;
}

void objloader_t::clean(){
}

objloader_t::~objloader_t(){
}

unsigned int objloader_t::loadTexture(std::string image_path){
	unsigned int num;
	glGenTextures(1,&num);
	SDL_Surface *img = nullptr;
	if(image_path.find_first_of(".bmp") != std::string::npos){
		img = SDL_LoadBMP(image_path.c_str());
	}else{
		printf("Could not load the file '%s': the filetype is not supported\n", image_path.c_str());
		assert(false);
		return -1;
	}
	glBindTexture(GL_TEXTURE_2D,num);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);
	glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	SDL_FreeSurface(img);
	texture.push_back(num);
	return num;
}

objloader_t::objloader_t(){
	//at default we set all booleans to false, so we don't use anything
	ismaterial=false;
	isnormals=false;
	istexture=false;
}

void model_load(model_t *a, std::string file){
	assert(a != nullptr || a != NULL);
	bool loadable = false;
	if(a->entire_object_file == ""){
		if(file != ""){
			std::ifstream in(file);
			if(in.is_open()){
				char data[512];
				while(in.getline(data, 512)){
					a->entire_object_file += (std::string)data + "\n";
	                                std::string data_2;
					std::stringstream ss;
					ss << data;
					ss >> data >> data_2;
					if((std::string)data == (std::string)"mtllib"){
						printf("Processing a material file\n");
						std::ifstream in_(data_2);
						a->entire_material_file.push_back("");
						const unsigned long int material_pos = a->entire_material_file.size()-1;
						while(in_.getline(data, 512)){
							a->entire_material_file[material_pos] += (std::string)data + "\n";
						}
					}
				}
			}
			loadable = true;
		}else{
			loadable = false;
		}
	}else{
		loadable = true;
	}
	if(likely(loadable)){
		objloader_t obj_loader;
		obj_loader.load(a);
	}
}
