#include "render_main.h"
#include "../input/input_main.h"

extern unsigned long int tick;
extern bool once_per_second;

void model_render(model_t*);


void render_rules_t::blank(){
	x_res = 640;
	y_res = 480;
	fov = 90;
	near_fov = (float).05;
	far_fov = (float)1500;
	enable_hud = enable_view = enable_grid = true;
	fullscreen = false;
	window_title = (char*)"Czech_mate Technical Demonstration";
	vsync = false;
}

render_rules_t::render_rules_t(){
	blank();
}

render_rules_t::~render_rules_t(){
	blank();
}

void render_t::init_subsystems(){
	printf("Using OpenGL 1.1\n");
	data = new render_data_t;
	render_opengl_dep = new render_opengl_dep_t(data); // make this better
	render_screen = data->screen;
	rules = data->rules; // backwards compatibility
}

void render_t::init_parse_parameters(int argc, char** argv){
	data = new render_data_t;
	for(int i = 0;i < argc;i++){
		if(strncmp((char*)argv[i],(char*)"--render-rules-x-res",strlen((char*)argv[i])) == 0){
			data->rules->x_res = (unsigned int)atoi((char*)argv[i+1]);
		}else if(strncmp((char*)argv[i],(char*)"--render-rules-y-res",strlen((char*)argv[i])) == 0){
			data->rules->y_res = (unsigned int)atoi((char*)argv[i+1]);
		}else if(strncmp((char*)argv[i],(char*)"--render-rules-fullscreen",strlen((char*)argv[i])) == 0){
			data->rules->fullscreen = true;
		}else if(strncmp((char*)argv[i],(char*)"--render-rules-vsync",strlen((char*)argv[i])) == 0){
			data->rules->vsync = true;
		}
	}
}

void render_t::init_generate_window(){
	if(render_opengl_dep != nullptr){
		render_opengl_dep->generate_window();
	}
}

void render_t::init_load_models(){
	#ifdef __linux
		system("find | grep obj > model_search_output");
		std::ifstream in("model_search_output");
		if(in.is_open()){
			char data[512];
			while(in.getline(data, 512)){
				model_t *model = new model_t();
				model_load(model, data);
			}
		}
	#endif
}

render_t::render_t(int argc, char** argv){
	render_opengl_dep = nullptr;
	init_parse_parameters(argc, argv);
	init_subsystems();
	init_generate_window();
	init_load_models();
}

void render_t::loop_render_buffer(){
	if(render_opengl_dep != nullptr){
		render_opengl_dep->loop_render_buffer();
	}
}

void render_t::loop_update(){
	if(render_opengl_dep != nullptr){
		render_opengl_dep->loop_update();
	}
}

void render_t::loop_init(){
	if(render_opengl_dep != nullptr){
		render_opengl_dep->loop_init();
	}
}

void render_t::loop_render_screen(){
	if(render_opengl_dep != nullptr){
		render_opengl_dep->loop_render_screen();
	}
}

int render_t::loop(coord_t *a){ // TODO: Divide this up into smaller chunks
	int return_value = 0;
	if(a != nullptr || a != NULL){
		data->coord = a;
		loop_init();
		loop_update();
		loop_render_buffer();
	}
	loop_render_screen(); // updates the screen
	return return_value;
}

render_t::~render_t(){
	if(render_opengl_dep != nullptr){
		delete render_opengl_dep;
		render_opengl_dep = nullptr;
	}
}

static inline void model_render_normal(coordinate *normal){
	glNormal3f(normal->x,normal->y,normal->z);
}

static inline void model_render_vertex(material *material, texcoord *texcoord, coordinate *vertex){
	assert(vertex != nullptr);
	if(material != nullptr && material->texture != -1 && texcoord != nullptr){
		glTexCoord2f(texcoord->u,texcoord->v);
	}
	glVertex3f(vertex->x, vertex->y, vertex->z);
}

void model_render(model_t *model){
	assert(model != nullptr || model != NULL);	
	const unsigned long int model_faces_size = model->faces.size();
	if(unlikely(model_faces_size == 0)){
		// only works if the entire_object_file has been loaded (sent from server)
		model_load(model);
	}
	std::vector<std::string*> coord	= model->coord;
	std::vector<coordinate*> vertex	= model->vertex;
	std::vector<face*> faces = model->faces;
	std::vector<coordinate*> normals = model->normals;
	std::vector<unsigned int> texture = model->texture;
	std::vector<unsigned int> lists	= model->lists;
	std::vector<material*> materials = model->materials;
	std::vector<texcoord*> texturecoordinate = model->texturecoordinate;
	bool ismaterial = model->ismaterial;
	bool isnormals = model->isnormals;
	bool istexture = model->istexture;
	int last = -1;
	glPushMatrix();
	for(unsigned long int i = 0;i < model_faces_size;i++){
		int mat = model->faces[i]->mat;
		const int *faces_ = model->faces[i]->faces;
		int facenum = model->faces[i]->facenum;
		int *texcoord_ = model->faces[i]->texcoord;
		if(last != mat && ismaterial){
			float diffuse[] = {materials[mat]->dif[0], materials[mat]->dif[1], materials[mat]->dif[2], 1.0};
			float ambient[] = {materials[mat]->amb[0], materials[mat]->amb[1], materials[mat]->amb[2], 1.0};
			float specular[] = {materials[mat]->spec[0], materials[mat]->spec[1], materials[mat]->spec[2], 1.0};
			glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
			glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
			glMaterialf(GL_FRONT,GL_SHININESS,materials[mat]->ns);
			last = mat;
			if(materials[mat]->texture == -1){
				glDisable(GL_TEXTURE_2D);
			}else{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, (GLuint)materials[mat]->texture);
			}
		}
		if(unlikely(faces[i]->four)){
			glBegin(GL_QUADS);
				if(isnormals) model_render_normal(normals[facenum-1]);
				for(unsigned int n = 0;n < 4;n++){
					material *local_mat = nullptr;
					if(ismaterial && mat < (int)materials.size()){
						local_mat = materials[mat];
					}
					texcoord *local_texcoord = nullptr;
					if(istexture && texcoord_[n]-1 < (int)texturecoordinate.size()){
						local_texcoord = texturecoordinate[texcoord_[n]-1];
					}
					model_render_vertex(local_mat, local_texcoord, vertex[faces_[n]-1]);
				}
			glEnd();
			printf("Rendered a quad\n");
		}else{
			glBegin(GL_TRIANGLES);
				if(isnormals) model_render_normal(normals[facenum-1]);
				for(unsigned int n = 0;n < 3;n++){
					material *local_mat = nullptr;
					if(ismaterial && mat < (int)materials.size()){
						local_mat = materials[mat];
					}
					texcoord *local_texcoord = nullptr;
					if(istexture && texcoord_[n]-1 < (int)texturecoordinate.size()){
						local_texcoord = texturecoordinate[texcoord_[n]-1];
					}
					model_render_vertex(local_mat, local_texcoord, vertex[faces_[n]-1]);
				}
			glEnd();
			printf("Rendered triangle\n");
		}
	}
	glPopMatrix();
}
