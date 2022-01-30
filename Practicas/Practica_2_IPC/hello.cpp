#include <iostream>
#include <unistd.h>


int main(int argc, const char** argv){
	std::cout << getpid() << " - " << getppid() << std::endl;
	if(argc == 2){
		std::cout << "Hello " << argv[1] << "!" << std::endl;
	}else{
		std::cout << "Hello World! " << std::endl;
	}
	return 0;
}
