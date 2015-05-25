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
#include "../render/render_main.h"
#include "server_console.h"
#include "server_net.h"
#include "server_gametype.h"

/*
TODO:
	Make a singlethreaded version. The only thread running will be running scanf and putting that information into an std::string.
*/

#define CONSOLE_INPUT_ARRAY_SIZE 65536

static void settings_engine();
static void class_engine();
static void terminate_engine();
static void render_object_engine();
static void input_render_object_engine(input_t*, coord_t*, float);
static void int_modifier_engine(int*);
static void double_modifier_engine(long double*);
static void string_modifier_engine(std::string*);
static std::thread *console_thread;
static char console_input_array[CONSOLE_INPUT_ARRAY_SIZE];
static std::string user_input;
static std::mutex user_input_lock;
extern net_t *net;
extern bool terminate;
static array_id_t last_gen_id = -1;
static void update_last_gen_id(array_id_t);
static array_id_t pull_user_input_id(array_t*, std::string);
static array_id_t pull_last_gen_id(array_t*);
static void loop_code_engine();

inline static void prompt_for_item(std::string prompt){
	std::cout << prompt;
	scanf("%65535s", console_input_array);
}

static void update_last_gen_id(array_id_t id){
	last_gen_id = id;
}

static array_id_t pull_user_input_id(array_t *tmp_array, std::string print_string = "ID"){
	printf("%s\n", print_string.c_str());
	prompt_for_item(print_string);
	if((std::string)console_input_array == "last"){
		return pull_last_gen_id(tmp_array);
	}
	array_id_t tmp = std::stoi(console_input_array);
	update_last_gen_id(tmp);
	return pull_last_gen_id(tmp_array); // assuring that the input ID is valid
}

static array_id_t pull_last_gen_id(array_t *tmp){
	if(likely(last_gen_id != -1)){
		if(likely((tmp = find_array_pointer(last_gen_id)) != nullptr)){
			tmp->data_lock.unlock();
			return last_gen_id;
		}else{
			printf("ID is no longer valid\n");
			return pull_user_input_id(tmp);
		}
	}else{
		tmp = nullptr;
		return last_gen_id;
	}
}

void console_init(){
	console_thread = new std::thread(console_engine);
}

void console_engine(){
	while(terminate == false){
		printf("]");
		prompt_for_item("");
		if((std::string)console_input_array == "settings"){
			settings_engine();
		}else if((std::string)console_input_array == "class"){
			class_engine();
		}else if((std::string)console_input_array == "terminate"){
			terminate_engine();
		}else if((std::string)console_input_array == "render" || (std::string)console_input_array == "render_object"){
			render_object_engine();
		}else if((std::string)console_input_array == "loop"){
			loop_code_engine();
		}
	}
}

void console_close(){}

#define INFO_INT_BIT 0
#define INFO_STRING_BIT 1
#define INFO_DOUBLE_BIT 2

static void loop_code_engine(){
	prompt_for_item("Code to run: ");
	std::string code = console_input_array;
	prompt_for_item("Times to loop the code: ");
	int64_t iterations = std::stoi(console_input_array);
	if(code == "spawn_coord"){
		new coord_t[iterations];
	}
}

static void input_render_object_engine(input_t *input, coord_t *coord, float time){
	if(input == nullptr){
		printf("Input render engine called with a null input engine\n");
		return;
	}
	if(coord == nullptr){
		printf("Input render engine called with a null coord engine\n");
		return;
	}
	const long double mod = time;
	coord->array.data_lock.lock();
	if(input->query_key(SDL_SCANCODE_W)){
		coord->set_y_angle(true, mod);
	}
	if(input->query_key(SDL_SCANCODE_A)){
		coord->set_x_angle(true, -mod);
	}
	if(input->query_key(SDL_SCANCODE_S)){
		coord->set_y_angle(true, -mod);
	}
	if(input->query_key(SDL_SCANCODE_D)){
		coord->set_x_angle(true, mod);
	}
	int64_t coordinate_to_use = 0;
	if(input->query_key(SDL_SCANCODE_X)){
		SET_BIT(coordinate_to_use, 0, 1);
	}
	if(input->query_key(SDL_SCANCODE_Y)){
		SET_BIT(coordinate_to_use, 1, 1);
	}
	if(input->query_key(SDL_SCANCODE_Z)){
		SET_BIT(coordinate_to_use, 2, 1);
	}
	if(input->query_key(SDL_SCANCODE_F)){
		if(CHECK_BIT(coordinate_to_use, 0)){
			coord->x += mod;
		}
		if(CHECK_BIT(coordinate_to_use, 1)){
			coord->y += mod;
		}
		if(CHECK_BIT(coordinate_to_use, 2)){
			coord->z += mod;
		}
	}
	if(input->query_key(SDL_SCANCODE_C)){
		if(CHECK_BIT(coordinate_to_use, 0) == 1){
			coord->x -= mod;
		}
		if(CHECK_BIT(coordinate_to_use, 1) == 1){
			coord->y -= mod;
		}
		if(CHECK_BIT(coordinate_to_use, 2) == 1){
			coord->z -= mod;
		}
	}
	coord->array.data_lock.unlock();
}

static void render_object_engine(){
	render_t *render = new render_t(argc_, argv_);
	input_t *input = new input_t(argc_, argv_, (new input_keyboard_map_t)->array.id);
	//coord_t *coord = new coord_t;g
	coord_t coord[2];
	coord[0].mobile = coord[1].mobile = false;
	coord[0].model_id = pull_user_input_id((array_t*)nullptr);
	long double old_time = get_time(), time = 0;
	while(infinite_loop()){
		input_render_object_engine(input, &coord[0], time);
		input->loop();
		coord[0].print();
		coord[1].print();
		render->loop(&coord[1]);
		time = get_time()-old_time;
		old_time = get_time();
	}
	delete render;
	delete input;
	render = nullptr;
	input = nullptr;
}

static void restart_terminate_engine(){
	util_shell(UTIL_SHELL_DELETE, "server_state.save");
	std::ofstream out("server_state.save");
	if(out.is_open()){
		const unsigned long int64_t array_size = array_vector.size();
		array_lock.lock();
		for(unsigned long int64_t i = 0;i < array_size;i++){
			out << array_vector[i]->gen_updated_string(UINT_MAX) << "\n";
		}
		array_lock.unlock();
		out.close();
		terminate = true;
	}else{
		printf("Could not write to the server_save file, not restarting\n");
	}
}

static void terminate_engine(){
	prompt_for_item("'restart' or 'quit'\n");
	if((std::string)console_input_array == "restart"){
		restart_terminate_engine();
	}else if((std::string)console_input_array == "quit"){
		terminate = true;
	}
}

static void new_class_engine(){
	array_t *array_of_new_item = nullptr;
	prompt_for_item("Gametype: ");
	if((std::string)console_input_array == "coord_t"){
		array_of_new_item = &((new coord_t)->array);
	}else if((std::string)console_input_array == "model_t"){
		array_of_new_item = &((new model_t)->array);
	}else if((std::string)console_input_array == "gametype_t"){
		array_of_new_item = &((new gametype_t)->array);
	}
	printf("Generated a %s with an ID of %d\n", array_of_new_item->data_type.c_str(), array_of_new_item->id);
	update_last_gen_id(array_of_new_item->id);
}

static void mod_class_engine(){
	array_t *selected = nullptr;
	pull_user_input_id(selected);
	selected->print();
	prompt_for_item("VECTOR ('int_array', 'long_double_array', 'string_array')\n");
	printf("ENTRY\n");
	int64_t position = 0;
	scanf("%d", &position);
	if((std::string)console_input_array == "int_array"){
		selected->int_lock.lock();
		int64_t *tmp = selected->int_array[position];
		selected->int_lock.unlock();
		int_modifier_engine(tmp);
	}else if((std::string)console_input_array == "long_double_array"){
		selected->long_double_lock.lock();
		long double *tmp = selected->long_double_array[position];
		selected->long_double_lock.unlock();
		double_modifier_engine(tmp);
	}else if((std::string)console_input_array == "string_array"){
		selected->string_lock.lock();
		std::string *tmp = selected->string_array[position];
		selected->string_lock.unlock();
		string_modifier_engine(tmp);
	}
	update_last_gen_id(selected->id);
}

static void del_class_engine(){
	array_t *array_to_delete = nullptr;
	pull_user_input_id(array_to_delete);
	delete_array_and_pointer(array_to_delete);
}

static void list_ext_class_engine(){
	prompt_for_item("'type', 'id', or 'all' search\n");
	std::vector<array_t*> *tmp_array_vector = nullptr;
	bool allocated_memory = false;
	if((std::string)console_input_array == "type"){
		//allocated_memory = true;
		//tmp_array_vector = new std::vector<array_t*>;
		//*tmp_array_vector = (std::vector<array_t*>)all_entries_of_type(console_input_array);
	}else if((std::string)console_input_array == "id"){
		allocated_memory = true;
		tmp_array_vector = new std::vector<array_t*>;
		array_t *new_array = nullptr;
		pull_user_input_id(new_array);
		tmp_array_vector->push_back(new_array);
	}else if((std::string)console_input_array == "all"){
		tmp_array_vector = &array_vector;
	}else return;
	const unsigned long int64_t tmp_array_vector_size = tmp_array_vector->size();
	for(unsigned long int64_t i = 0;i < tmp_array_vector_size;i++){
		(*tmp_array_vector)[i]->print();
	}
}

static void patch_ext_class_engine(){
	prompt_for_item("Filename: ");
	std::ifstream in((std::string)console_input_array);
	std::vector<std::string> tmp_vector;
	if(in.is_open()){
		while(in.getline(console_input_array, CONSOLE_INPUT_ARRAY_SIZE)){
			tmp_vector.push_back(console_input_array);
		}
		const unsigned long int64_t tmp_vector_size = tmp_vector.size();
		for(unsigned long int64_t i = 0;i < tmp_vector_size;i++){
			update_class_data(tmp_vector[i], UINT_MAX);
			update_progress_bar((long double)((long double)i/(long double)tmp_vector_size)*(long double)100);
		}
		printf("Finished with the patch.\n");
	}else{
		printf("Cannot open the patch, not applying anything.\n");
	}
}

static void export_class_engine(){
	array_id_t export_id = pull_user_input_id(nullptr, "ID to export: ");
	prompt_for_item("Filename: ");
	std::ofstream out(console_input_array);
	if(out.is_open()){
		array_t *tmp = find_array_pointer(export_id);
		out << tmp->gen_updated_string(UINT_MAX);
		tmp->data_lock.unlock();
		out.close();
	}else{
		printf("Could not open the file\n");
	}
}

static void class_engine(){
	prompt_for_item("'new', 'mod', 'del', 'list', or 'patch'?\n");
	if((std::string)console_input_array == "new"){
		new_class_engine();
	}else if((std::string)console_input_array == "mod"){
		mod_class_engine();
	}else if((std::string)console_input_array == "del"){
		del_class_engine();
	}else if((std::string)console_input_array == "list"){
		list_ext_class_engine();
	}else if((std::string)console_input_array == "patch" || (std::string)console_input_array == "import"){
		patch_ext_class_engine();
	}else if((std::string)console_input_array == "export"){
		export_class_engine();
	}
}

static void int_modifier_engine(int* pointer){
	prompt_for_item("'bit', 'constant', or 'math' modification?\n");
	if((std::string)console_input_array == "bit"){
		printf("BIT NEW_VALUE\n");
		int64_t bit, value;
		scanf("%d %d", &bit, &value);
		SET_BIT(*pointer, bit, value);
		printf("pointer's new value: %d\n", *pointer);
		return;
	}else if((std::string)console_input_array == "constant"){
		printf("NEW_VALUE\n");
		int64_t new_value;
		scanf("%d", &new_value);
		*pointer = new_value;
	}else if((std::string)console_input_array == "math"){
		printf("'add', 'sub', 'mul' or 'div' is the first keyword, the value is the second one\n");
		printf("KEYWORD VALUE\n");
		float modifier;
		scanf("%3s %f", console_input_array, &modifier);
		if((std::string)console_input_array == "add"){
			*pointer += modifier;
		}else if((std::string)console_input_array == "sub"){
			*pointer -= modifier;
		}else if((std::string)console_input_array == "mul"){
			*pointer *= modifier;
		}else if((std::string)console_input_array == "div"){
			if(modifier == 0){
				modifier += 0.00000001;
			}
			*pointer /= modifier;
		}
	}
}

static void string_modifier_engine(std::string *pointer){
	printf("constant modification is the only available one here\n");
	prompt_for_item("NEW_CONSTANT\n");
	*pointer = console_input_array;
}

static void double_modifier_engine(long double *pointer){
	prompt_for_item("'constant' or 'math'\n");
	if((std::string)console_input_array == "constant"){
		prompt_for_item("NEW_VALUE");
		// check to see that the input is valid so we don't blank the variable on accident
		*pointer = strtold(console_input_array, nullptr);
	}else if((std::string)console_input_array == "math"){
		printf("'add', 'sub', 'mul', or 'div' is the first keyboard, the value is the second one\n");
		float modifier;
		scanf("%3s %f", console_input_array, &modifier);
		if((std::string)console_input_array == "add"){
			*pointer += modifier;
		}else if((std::string)console_input_array == "sub"){
			*pointer -= modifier;
		}else if((std::string)console_input_array == "mul"){
			*pointer *= modifier;
		}else if((std::string)console_input_array == "div"){
			*pointer /= modifier;
		}
	}
}

static void settings_engine(){
	void *pointer_to_data;
	int64_t info_about_pointer = 0;
	prompt_for_item("Variable to modify: ");
	if((std::string)console_input_array == "loop_settings"){
		pointer_to_data = &loop_settings;
		SET_BIT(info_about_pointer, INFO_INT_BIT, 1);
	}else if((std::string)console_input_array == "net_loop_settings"){
		pointer_to_data = &net_loop_settings;
		SET_BIT(info_about_pointer, INFO_INT_BIT, 1);
	}
	if(CHECK_BIT(info_about_pointer, INFO_INT_BIT) == 1){
		int_modifier_engine((int*)pointer_to_data);
	}else if(CHECK_BIT(info_about_pointer, INFO_STRING_BIT) == 1){
		string_modifier_engine((std::string*)pointer_to_data);
	}else if(CHECK_BIT(info_about_pointer, INFO_DOUBLE_BIT) == 1){
		double_modifier_engine((long double*)pointer_to_data);
	}
}

void input_thread(){
	while(true){
	}
}
