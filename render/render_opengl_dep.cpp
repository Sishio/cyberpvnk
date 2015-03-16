#include "render_main.h"
#include "../input/input_main.h"

extern unsigned long int tick;
extern bool once_per_second;

static int render_gen_vbo[32] = {-1};

extern void model_render(model_t*);

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

void render_opengl_dep_t::generate_window(){
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	unsigned int parameters_sdl = 0;
	if(data->rules->fullscreen){
		parameters_sdl = SDL_WINDOW_FULLSCREEN_DESKTOP;
	}else{
		parameters_sdl = SDL_WINDOW_SHOWN;
	}
	parameters_sdl |= SDL_WINDOW_OPENGL;
	data->screen = SDL_CreateWindow((char*)data->rules->window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,(int)data->rules->x_res,(int)data->rules->y_res,parameters_sdl);
	if(data->screen == NULL){
		printf("Could not open the window, must be running this as a dev test over SSH or in a terminal without X11 access. Terminating render module\n");
		printf("Re-run with --render-disable\n");
	}
	//SDL_ShowCursor(SDL_DISABLE);

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
	data->context = SDL_GL_CreateContext(data->screen);
	SDL_GL_MakeCurrent(data->screen, data->context);
}

void render_opengl_dep_t::loop_render_buffer(){
/*	coord_t *coord = data->coord;
	if(coord == nullptr){
		return;
	}
	glRotatef((GLfloat)coord->x_angle, (GLfloat)1.0, (GLfloat)0.0, (GLfloat)0.0);
	glRotatef((GLfloat)coord->y_angle, (GLfloat)0.0, (GLfloat)1.0, (GLfloat)0.0);
	glTranslatef((GLfloat)-coord->x,(GLfloat)-coord->y,(GLfloat)-coord->z);
        std::vector<void*> coord_vector = all_pointers_of_type("coord_t");
	const unsigned long int coord_size = coord_vector.size();
	for(unsigned long int i = 0;i < coord_size;i++){
		coord_t *local_coord = (coord_t*)coord_vector[i];
		if(local_coord->model_id != 0){
			glPushMatrix();
			glTranslatef((GLfloat)local_coord->x, (GLfloat)local_coord->y, (GLfloat)local_coord->z);
			glRotatef((GLfloat)local_coord->x_angle, (GLfloat)1.0, (GLfloat)0.0, (GLfloat)0.0);
			model_t *model = (model_t*)find_pointer(local_coord->model_id);
			if(model != nullptr){
				model_render(model);
			}
			glPopMatrix();
		}
	}*/
	glBegin(GL_POLYGON);
	glVertex2f(.5, .5);
	glVertex2f(.5, -.5);
	glVertex2f(-.5, -.5);
	glVertex2f(-.5, .5);
	glEnd();
}

void render_opengl_dep_t::loop_update(){
	int data_[2] = {0};
	data->rules->x_offset = data->rules->y_offset = 0;
	SDL_GetWindowSize(data->screen,&data_[0],&data_[1]);
	if(data_[0] > 0 && data_[1] > 0){
		data->rules->x_res = (unsigned int)data_[0];
		data->rules->y_res = (unsigned int)data_[1];
	}
}

void render_opengl_dep_t::loop_init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspectRatio;
	if(data->rules->x_res > data->rules->y_res){
		aspectRatio = data->rules->x_res/data->rules->y_res;
	}else{
		aspectRatio = data->rules->y_res/data->rules->x_res;
	}
	GLfloat fH = (GLfloat)tan(float(data->rules->far_fov/PI_360))*data->rules->near_fov;
	GLfloat fW = fH * aspectRatio;
	glFrustum(-fW, fW, -fH, fH, data->rules->near_fov, data->rules->far_fov);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void render_opengl_dep_t::loop_render_screen(){
	SDL_GL_SwapWindow(data->screen);
}

void render_opengl_dep_t::destroy_window(){
	SDL_GL_DeleteContext(data->context);
	SDL_DestroyWindow(data->screen);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

render_opengl_dep_t::render_opengl_dep_t(render_data_t *data_){
	data = data_;
}

render_opengl_dep_t::~render_opengl_dep_t(){
	destroy_window();
	data = nullptr;
}
