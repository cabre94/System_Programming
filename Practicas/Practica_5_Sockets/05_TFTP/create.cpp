#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

int main(){

	ofstream myfile;
	myfile.open("test.txt");

	for(long i = 0; i < (1<<14); ++i){
		myfile << (i+1);
		myfile << '\t';
		for(int j=0; j < 100; j++){
			myfile << (char) ('a' +(rand() % 26));
		}
		myfile << '\n';
	}

	myfile.close();

	return 0;
}