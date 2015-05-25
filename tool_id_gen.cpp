#include "cstdio"
#include "iostream"
#include "fstream"
#include "cmath"
#include "cstdlib"
#include "cstring"

#define CHECK_BIT(var, pos) ((var & ( 1 << pos )) >> pos)
#define SET_BIT(var,pos,val) (var ^= (-val ^ var) & (1 << pos))
#define FLIP_BIT(var, pos) (var ^= (1 << pos))	

unsigned int sum_of_string(std::string entropy){
	int return_value = 0;
	for(unsigned long int i = 0;i < entropy.size();i++){
		return_value += static_cast<int>(entropy[i]);
	}
	return return_value;
}

std::string int_to_binary(int val){
	std::string return_value;
	unsigned int mask = 1 << (sizeof(int)*8-1);
	for(int i = 0;i < sizeof(int)*8;i++){
		if((val & mask) == 0){
			return_value += "0";
		}else{
			return_value += "1";
		}
		mask >>= 1;
	}
	return return_value;
}

int main(){
	int id;
	std::string entropy;
	std::cout << "Strip ID: ";
	std::cin >> id;
	std::cout << "Type random characters to generate entropy: ";
	std::cin >> entropy;
	std::srand(sum_of_string(entropy));
	for(unsigned long int i = 16;i <= 32;i++){
		SET_BIT(id, i, std::rand()%2);
		std::srand(static_cast<unsigned int>(std::rand()+i));
	}
	std::cout << "ID: " << id << std::endl << "Binary ID: " << int_to_binary(id) << std::endl;
	return 0;
}
