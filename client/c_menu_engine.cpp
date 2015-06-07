#include "c_menu_engine.h"

int_ menu_loop(){
	printf("Czech_mate\n");
	printf("(1) Connect to server\n");
	printf("(2) Net module + test logic\n");
	printf("(3) Exit\n");
	printf("(4) Only run the console\n");
	int choice;
	scanf("%d",&choice);
	return choice;
}
