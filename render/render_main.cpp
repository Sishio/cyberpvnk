#include "render_main.h"
#include "../input/input_main.h"

extern unsigned long int tick;
extern bool once_per_second;

void model_render(model_t*);

static int render_gen_vbo[32] = {-1};

int render_generate_shape(int shape){
	int return_value = 0;
	switch(shape){
	case RENDER_GENERATE_SHAPE_PLANE: // TODO: just use a call list (is there a way to assign a number to it so I can use macros?)
		break;
	case RENDER_GENERATE_SHAPE_GRID:
		if(render_gen_vbo[RENDER_GENERATE_SHAPE_GRID] == -1){
			glBegin(GL_LINES);
			for(int i = 0; i <= 100; i += 10){
				glVertex3i(i, 0, 0);
				glVertex3i(i, 100, 0);
				glVertex3i(0, i, 0);
				glVertex3i(100, i, 0);
			}
			glEnd();
		}else{
			// render the VBO
		}
		break;
	default:
		printf("The shape parameter does not match anything\n");
		return_value = -1;
	}
	return return_value;
}

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

void render_rules_t::init(){
	blank();
}

void render_rules_t::close(){
	blank();
}

void render_t::blank(){
}

void render_t::init_subsystems(){
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	rules.init();
}

void render_t::init_configure_buffer(){
}

void render_t::init_parse_parameters(int argc, char** argv){
	for(int i = 0;i < argc;i++){
		if(strncmp((char*)argv[i],(char*)"--render-rules-x-res",strlen((char*)argv[i])) == 0){
			rules.x_res = (unsigned int)atoi((char*)argv[i+1]);
		}else if(strncmp((char*)argv[i],(char*)"--render-rules-y-res",strlen((char*)argv[i])) == 0){
			rules.y_res = (unsigned int)atoi((char*)argv[i+1]);
		}else if(strncmp((char*)argv[i],(char*)"--render-rules-fullscreen",strlen((char*)argv[i])) == 0){
			rules.fullscreen = true;
		}else if(strncmp((char*)argv[i],(char*)"--render-rules-vsync",strlen((char*)argv[i])) == 0){
			rules.vsync = true;
		}
	}
}

void render_t::init_generate_window(){
	unsigned int parameters_sdl = 0;
	if(rules.fullscreen){
		parameters_sdl = SDL_WINDOW_FULLSCREEN_DESKTOP;
	}else{
		parameters_sdl = SDL_WINDOW_SHOWN;
	}
	parameters_sdl |= SDL_WINDOW_OPENGL;
	render_screen = SDL_CreateWindow((char*)rules.window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,(int)rules.x_res,(int)rules.y_res,parameters_sdl);
	if(render_screen == NULL){
		printf("Could not open the window, must be running this as a dev test over SSH or in a terminal without X11 access. Terminating render module\n");
		printf("Re-run with --render-disable\n");
	}
	SDL_ShowCursor(SDL_DISABLE);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(1,1,1,1);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_NORMALIZE);
	render_context = SDL_GL_CreateContext(render_screen);
	SDL_GL_MakeCurrent(render_screen, render_context);
}

void render_t::init_load_models(){
}

render_t::render_t(int argc, char** argv){
	init_configure_buffer();
	init_subsystems();
	blank();
	init_generate_window();
	init_parse_parameters(argc,argv);
	printf("x_res:%u\ty_res:%u\n",rules.x_res, rules.y_res);
	// OpenGL powers go under the rules
}

void render_t::loop_render_buffer(){
	glRotatef(coord->x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(coord->y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(-coord->x,-coord->y,-coord->z);
	const unsigned int array_size = array_vector.size();
	for(unsigned int i = 0;i < array_size;i++){
		if(array_vector[i] != nullptr && array_vector[i]->data_type == "render_buffer_t"){
		  coord_t *local_coord = (coord_t*)find_array_pointer(((render_buffer_t*)(array_vector[i]))->coord_id);
			glPushMatrix();
			glTranslatef(local_coord->x, local_coord->y, local_coord->z);
			glRotatef(local_coord->x_angle,1.0f,0.0f,0.0f);
			printf("local_coord->model_id: %d\n",local_coord->model_id);
			model_t *model = (model_t*)find_array_pointer(local_coord->model_id);
			if(model == nullptr){
			  model = (model_t*)find_array_pointer(((render_buffer_t*)(array_vector[i]))->model_id);
			}
			model_render(model);
			glPopMatrix();
		}
	}
}

void render_t::loop_update(){
	int data[2] = {0};
	rules.x_offset = rules.y_offset = 0;
	SDL_GetWindowSize(render_screen,&data[0],&data[1]);
	if(data[0] > 0 && data[1] > 0){
		rules.x_res = (unsigned int)data[0];
		rules.y_res = (unsigned int)data[1];
	}
}

void render_t::loop_init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	assert(rules.x_res > rules.y_res);
	GLfloat aspectRatio = rules.x_res/rules.y_res;;
	GLfloat fH = tan(float(rules.far_fov/PI_360))*rules.near_fov;
	GLfloat fW = fH * aspectRatio;
	glFrustum(-fW, fW, -fH, fH, rules.near_fov, rules.far_fov);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void render_t::loop_render_screen(){
	SDL_GL_SwapWindow(render_screen);
}

int render_t::loop(coord_t *a){ // TODO: Divide this up into smaller chunks
	coord = a;
	assert(a != nullptr || a != NULL);
	int return_value = 0;
	loop_init();
	loop_update();
	loop_render_buffer();
	loop_render_screen();
	return return_value;
}

void render_t::close(){
	blank();
	SDL_GL_DeleteContext(render_context);
	render_context = nullptr;
	SDL_DestroyWindow(render_screen);
	render_screen = nullptr;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
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
		printf("Attempted to render an empty model\n");
		return;
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
		if(faces[i]->four){
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
		}
	}
	// is it possible to unbind a texture?
	glPopMatrix();
}
