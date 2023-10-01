#include "../inc/Emulator.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

unordered_map<unsigned int, int> loadImage(string imageName){

    ifstream img(imageName);

    string imgLine;
    unordered_map<unsigned int, int> result;

    if (!img.is_open()) {
        cout << "Error while opening file." << imageName << "\n";
        return {};
    }
    
    while(img){

        getline(img, imgLine);
        
        if(imgLine.size() == 0) break;

        unsigned int address;
    	istringstream iss(imgLine.substr(0, 8));
        iss >> hex >> address;
        
        unsigned int value;
    	istringstream vss(imgLine.substr(8, 8));
        vss >> hex >> value;
        
        result[address] = static_cast<int>(value & 0x000000ff);
        result[address + 1] = static_cast<int>((value >> 8) & 0x000000ff);
        result[address + 2] = static_cast<int>((value >> 16) & 0x000000ff);
        result[address + 3] = static_cast<int>((value >> 24) & 0x000000ff);
        
    }

    return result;
}


int main(int argc, char* argv[]){

    if(argc < 2){
		cout << "Bad arguments" << "\n";
		return -1;
	}
	
	string fileName = argv[1];
	unordered_map<unsigned int, int> memory = loadImage(fileName);
	
    cout << "===== EMULATOR STARTED =====\n";

	Emulator emulator(memory);
    emulator.emulate();
    
    return 0;
}
