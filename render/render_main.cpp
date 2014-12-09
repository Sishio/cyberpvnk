#include "render_main.h"
#include "../input/input_main.h"

extern unsigned long int tick;

static int render_gen_vbo[32] = {-1};

int render_generate_shape(int shape){
	int return_value = 0;
	//GLfloat grid_vertices[] = {1.0, 0.0, -1.0, 1.0, 0.0, 1.0, -1.0, 0.0, 1.0 -1.0, 0.0, -1.0};
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
	opengl_powers.vertex_buffer_objects = RENDER_OPENGL_POWER_USED;
	opengl_powers.gl_begin_end = RENDER_OPENGL_POWER_DEPRECATED;
}

void render_rules_t::init(){
	blank();
	opengl_powers.vertex_buffer_objects = RENDER_OPENGL_POWER_USED;
	printf("OpenGL VBO support: %d\n",opengl_powers.vertex_buffer_objects);
	// streamline this
}

void render_rules_t::close(){
	blank();
}

void render_t::blank(){
	coord = NULL;
}

void render_t::init_subsystems(){
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	rules.init();
}

void render_t::init_configure_buffer(){
	render_buffer = new render_buffer_t*[RENDER_BUFFER_SIZE];
	term_if_true(render_buffer == NULL,(char*)"render_buffer");
	for(unsigned int i = 0;i < RENDER_BUFFER_SIZE;i++){
		render_buffer[i] = NULL;
	}
	for(int i = 0;i < 32;i++){
		render_gen_vbo[i] = -1;
	}
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
	parameters_sdl |= SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	render_screen = SDL_CreateWindow((char*)rules.window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,(int)rules.x_res,(int)rules.y_res,parameters_sdl);
	term_if_true(render_screen == NULL,(char*)"render_screen");
	SDL_GL_SetSwapInterval(rules.vsync); // VSync
}

void render_t::init_load_models(){
/*	#ifdef _WIN32
		assert(false);
	#endif
	system("find models | grep obj > model_data");
	std::ifstream in("model_data");
	if(in.is_open() == false){
		printf("Could not open the renderer\n");
	}
	char data[512];
	while(in.getline(data,512)){
		printf("%s\n",data);
		model_t **a = (model_t**)allocate_model();
		int c = 0;
		for(int i = 0;i < MODEL_POINTER_SIZE;i++){
			model_t **b = &model_pointer[i];
			if(a == b) c = i;
		}
		model_pointer[c]->init();
		model_pointer[c]->load(data);
	}*/
}

render_t::render_t(int argc, char** argv){
	init_configure_buffer();
	init_subsystems();
	blank();
	init_parse_parameters(argc,argv);
	init_generate_window(); // also takes care of gl* defaults
	// OpenGL powers go under the rules
}

void render_t::loop_render_buffer(){
	if(coord != NULL){
		glRotatef(coord->x_angle, 1.0f, 0.0f, 0.0f);
		glRotatef(coord->y_angle, 0.0f, 1.0f, 0.0f);
		glTranslatef(-coord->x,-coord->y,-coord->z);
		for(unsigned int i = 0;i < RENDER_BUFFER_SIZE;i++){
			if(render_buffer[i] != NULL){
				coord_t *coord = render_buffer[i]->coord;
				glPushMatrix();
				glTranslatef(coord->x,coord->y,coord->z);
				glRotatef(coord->x_angle,1.0f,0.0f,0.0f);
				glRotatef(coord->y_angle,0.0f,1.0f,0.0f);
				//render_buffer[i]->model->render();
				if(render_buffer[i]->delete_on_render){
					delete render_buffer[i]->coord;
					delete render_buffer[i]->model;
				}
				render_buffer[i]->coord = coord = NULL;
				render_buffer[i]->model = NULL;
				delete render_buffer[i];
				render_buffer[i] = NULL;
				glPopMatrix();
			}else break;
		}
	}
}

void render_t::loop_update(){
	int data[2] = {0};
	rules.x_offset = rules.y_offset = 0;
	SDL_GetWindowSize(render_screen,&data[0],&data[1]);
	rules.x_res = (unsigned int)data[0];
	rules.y_res = (unsigned int)data[1];
	//printf("Res:%dx%d\n",render_rules.x_res,render_rules.y_res);
	//printf("Offset:%d&%d\n",render_rules.x_offset,render_rules.y_offset);
}

void render_t::loop_init(){
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	for(int i = 0;i < 2;i++){
		if(i == 0) glMatrixMode(GL_MODELVIEW);
		else glMatrixMode(GL_PROJECTION);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
	}
	GLfloat aspect_ratio;
	if(rules.x_res > rules.y_res){
		aspect_ratio = rules.x_res/rules.y_res;
	}else{
		aspect_ratio = rules.y_res/rules.x_res;
	}
	GLfloat fH = tanf((float)((float)rules.fov/PI_360)*(float)rules.near_fov);
	GLfloat fW = fH*aspect_ratio;
	glFrustum(-fW, fW, -fH, fH, rules.near_fov, rules.far_fov);
	term_if_true(glGetError(),(char*)"glGetError()");
	//printf("render_t::loop_init():%dx%d\n",render_rules.x_res,render_rules.y_res);
}

void render_t::loop_render_screen(){
	SDL_GL_SwapWindow(render_screen);
}

int render_t::loop(){ // TODO: Divide this up into smaller chunks
	int return_value = 0;
	const bool update_render_context = tick%100 == 0;
	if(update_render_context) render_context = SDL_GL_CreateContext(render_screen);
	loop_init();
	loop_update();
	loop_render_buffer();
	loop_render_screen();
	if(update_render_context) SDL_GL_DeleteContext(render_context);
	return return_value;
}

int render_t::write_to_buffer(render_buffer_t *a){
	int return_value = -1;
	for(unsigned int i = 0;i < RENDER_BUFFER_SIZE;i++){
		if(render_buffer[i] == NULL){
			render_buffer[i] = new render_buffer_t;
			render_buffer[i]->coord = a->coord;
			render_buffer[i]->model = a->model;
			render_buffer[i]->delete_on_render = a->delete_on_render;
			return_value = (int)i;
			break;
		}
	}
	return return_value;
}

void render_t::close(){
	blank();
	for(int i = 0;i < RENDER_BUFFER_SIZE;i++){
		if(render_buffer[i] != NULL){
			if(render_buffer[i]->delete_on_render){
				delete render_buffer[i]->coord;
				render_buffer[i]->coord = NULL;
				delete render_buffer[i]->model;
				render_buffer[i]->model = NULL;
			}
			delete render_buffer[i];
			render_buffer[i] = NULL;
		}
	}
	delete[] render_buffer;
	render_buffer = NULL;
	SDL_DestroyWindow(render_screen);
	render_screen = NULL;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
