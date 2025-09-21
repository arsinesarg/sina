#include <iostream>

int main(int argc, char** argv) {
	if(argc <= 1) {
	    std::cout<< "ERROR";
		return 1;
	}
	for(int i = argc - 1; i >= 1; --i) {
		std::cout << argv[i] << " ";
       	}
	return 0;
}