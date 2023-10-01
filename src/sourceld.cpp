#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>

#include "../inc/Linker.hpp"

using namespace std;

int checkArguments(int argc, char* argv[], string& ofile, vector<string>& ifiles, map<string, unsigned int>& mpSectionAddress){
	
	bool hexExists = false;
	
	for(int i = 1; i < argc; i++){
		
		if(strcmp(argv[i], "-o") == 0){
			i++;
			if(i >= argc) return -1;
			ofile = argv[i];
			continue;
		}
		
		string currArg = argv[i];
		
		if (currArg.find("-place") != string::npos) {
			
    		int pos = currArg.find("=");
    		string sectionAndAddress = currArg.substr(pos + 1);
    		
    		pos = sectionAndAddress.find("@");
    		string sectionName = sectionAndAddress.substr(0, pos);
    		string address = sectionAndAddress.substr(pos + 1);
    		
    		//convert to int
    		unsigned int addr; 
    		stringstream ss;
    		ss << hex << address;
    		ss >> addr;
    		
    		mpSectionAddress.insert(make_pair(sectionName, addr));
    		
			continue;	
		}
		
		if(strcmp(argv[i], "-hex") == 0){
			hexExists = true;
			continue;
		}
		
		ifiles.push_back(argv[i]);
	}
	
	if(hexExists == false) return -1;
	return 0;
}

int main(int argc, char* argv[]) {
	
	string ofile = "output.hex";
	vector<string> ifiles;
	map<string, unsigned int> mpSectionAddress;
	
	cout << "Linker started" << "\n";

	if(checkArguments(argc, argv, ofile, ifiles, mpSectionAddress) == -1){
		cout << "Bad arguments" << "\n";
		return -1;
	}

	cout << "Starting linking" << "\n";
	
	Linker linker(mpSectionAddress, ifiles, ofile);
	linker.link();
	
	return 0;
}
