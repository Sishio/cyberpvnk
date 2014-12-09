#include "net_store.h"

void store_coord_t(coord_t a, std::vector<coord_t*> *b, std::vector<coord_extra_t> *c){
	const int coord_size = b->size();
	for(int i = 0;i < coord_size;i++){
		if((*b)[i] != nullptr){
			if(a.array->id_match((*b)[i]->array->id)){
				memcpy(&(*b)[i],&a,sizeof(coord_t));
				(*c)[i].updated = true;
				return;
			}
		}
	}
	for(int i = 0;i < coord_size;i++){
		if((*b)[i] == nullptr){
			(*b)[i] = new coord_t;
			memcpy(&(*b)[i],&a,sizeof(coord_t));
			(*c)[i].updated = true;
			return;
		}
	}
	b->push_back(new coord_t);
	memcpy(&(*b)[coord_size+1],&a,sizeof(coord_t));
	coord_extra_t d;
	d.updated = true;
	c->push_back(d);
}

void store_model_t(model_t a, std::vector<model_t*> *b, std::vector<model_extra_t> *c){
	const int model_size = b->size();
	for(int i = 0;i < model_size;i++){
		if((*b)[i] != nullptr){
			if(a.array->id_match((*b)[i]->array->id)){
				memcpy(&(*b)[i],&a,sizeof(model_t));
				(*c)[i].updated = true;
				return;
			}
		}
	}
	for(int i = 0;i < model_size;i++){
		if((*b)[i] == nullptr){
			(*b)[i] = new model_t;
			memcpy(&(*b)[i],&a,sizeof(model_t));
			(*c)[i].updated = true;
			return;
		}
	}
	b->push_back(new model_t);
	memcpy(&(*b)[model_size+1],&a,sizeof(model_t));
	model_extra_t d;
	d.updated = true;
	c->push_back(d);
}

