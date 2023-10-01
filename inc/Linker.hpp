#ifndef _LINKER_HPP
#define _LINKER_HPP

#include "utils.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <algorithm>

using namespace std;

class Linker{
private:
	
	unordered_map<string, vector<uint8_t>> mpSectionData;
	map<string, unsigned int> mpSectionAddress;
	map<string, int> sections;
	vector<string> ifiles;
	map<string, vector<Symbol>> mpSymbolTable;
	map<string, vector<SectionHeaderElem>> mpSectionHeader;
	map<string, vector<RelocationRecord>> mpRelocationTable; //key: nameOfFile:RelocTableName;
	string ofile;
	ofstream file;
	fstream textFile;
	vector<pair<unsigned int, int>> outputData;
	unsigned int location = 0;
	map<string, vector<pair<string, int>>> mpSectionFilesOffsets; //to save the order of files; needed for determing phase 
	map<string, int> mpSectionOffset;


	vector<Symbol> symbolTable;
	//sectionName, finalPosition
	map<string, int> mpSectionPos;
	vector<SectionHeaderElem> finalSectionHeader;
	
	unordered_map<int, string> mpHex;
	
	void mapping();
	void determing();
	void resolving();
	void place();
	void outputToFiles();
	
	void readSymbolTable(ifstream& file, string fileName);
	void readSection(ifstream& file, string fileName, SectionHeaderElem& elem);
	int readNumberOfRelocationTables(ifstream& file, string fileName);
	void readRelocationTable(ifstream& file, string fileName);
	void readSectionHeader(ifstream& file, string fileName);
	
public:
	Linker(map<string, unsigned int>& mpSectionAddress, vector<string> ifiles, string ofile)
		:mpSectionAddress(mpSectionAddress), ifiles(ifiles), ofile(ofile){}
	
	void link();
};

#endif
