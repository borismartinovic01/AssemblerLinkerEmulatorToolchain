#include "../inc/Linker.hpp"

void Linker::link(){
	
	mpHex[0] = "0";
    mpHex[1] = "1";
    mpHex[2] = "2";
    mpHex[3] = "3";
    mpHex[4] = "4";
    mpHex[5] = "5";
    mpHex[6] = "6";
    mpHex[7] = "7";
    mpHex[8] = "8";
    mpHex[9] = "9";
    mpHex[10] = "a";
    mpHex[11] = "b";
    mpHex[12] = "c";
    mpHex[13] = "d";
    mpHex[14] = "e";
    mpHex[15] = "f";
	
	mapping();
	determing();
	resolving();

	// check if there are undefined symbol definitions

	for(auto& it: mpSymbolTable){
		
		vector<Symbol>& symbols = it.second;
		string currFileName = it.first;
		
		for(int i = 1; i < symbols.size(); i++){
			
			if(symbolTypeToString(symbols[i].type) == "NOTYP" && symbols[i].ndx == 0) {
				
				bool symbolIsDefined = false;

				for(auto& entry: symbolTable){
					if(entry.name == symbols[i].name){
						symbolIsDefined = true;
						break;
					}
				}
				if(symbolIsDefined == false){
					cout << "ERROR: " << "Undefined symbol: " << symbols[i].name << "\n";
					exit(-1);
				}
			}
		}
	}

	outputToFiles();

	
	
	//output symbolTable for debugging purposes

	for(Symbol sym: symbolTable){
		cout << sym.num << "\t";
		cout << sym.value << "\t";
		cout << sym.size << "\t";
		cout << sym.type << "\t";
		cout << sym.bind << "\t";
		cout << sym.ndx << "\t";
		cout << sym.name << "\n";
	}
}

void Linker::mapping(){
	
	for(string& ifile: ifiles){
		
		ifstream file;

		cout << ifile << "\n";
		file.open(ifile, std::ios::out | std::ios::binary);

		readSectionHeader(file, ifile);
		readSymbolTable(file, ifile);
	
		int numOfRelocationTables = readNumberOfRelocationTables(file, ifile);
		
		for(int i = 0; i < numOfRelocationTables; i++){
			readRelocationTable(file, ifile);
		}
		
		vector<SectionHeaderElem>& sections = mpSectionHeader.find(ifile)->second;
		for(SectionHeaderElem& elem: sections){
			
			readSection(file, ifile, elem);
		}
		
		file.close();

		// check if sections can be placed on desired locations

		for (auto& entry1 : mpSectionAddress) {
        string sectionName1 = entry1.first;
        unsigned int sectionAddress1 = entry1.second;
        unsigned int sectionSize1 = mpSectionData[sectionName1].size();

        for (auto& entry2 : mpSectionAddress) {
            if (entry1 == entry2) continue;

            string sectionName2 = entry2.first;
            unsigned int sectionAddress2 = entry2.second;
            unsigned int sectionSize2 = mpSectionData[sectionName2].size();

            if ((sectionAddress1 < sectionAddress2 + sectionSize2) &&
                (sectionAddress2 < sectionAddress1 + sectionSize1)) {
                cout << "ERROR: Sections " << sectionName1 << " and " << sectionName2 << " overlap." << "\n";
                exit(-1);
            }
        }
    }
		
	}
	
	place();
	
}

void Linker::determing(){
	
	string currSymbol = "undefined";
	Symbol symbol(0, 0, 0, 0, 0, -1, currSymbol);
	symbolTable.push_back(symbol);
	
	for(auto& it: mpSectionPos){
		
		Symbol newSymbol(symbolTable.size(), it.second, 0, 1, 0, symbolTable.size(), it.first);
		symbolTable.push_back(newSymbol);
		
		SectionHeaderElem elem;
		elem.num = newSymbol.ndx;
		elem.address = newSymbol.value;
		elem.size = mpSectionData[newSymbol.name].size();
		elem.name = newSymbol.name;
		elem.currSize = 0;
		
		finalSectionHeader.push_back(elem);
	}
	
	//iterate through every symbolTable and look for noType where ndx is defined

	for(auto& it: mpSymbolTable){
		
		vector<Symbol>& symbols = it.second;
		string currFileName = it.first;
		
		for(int i = 1; i < symbols.size(); i++){
			
			if(symbolTypeToString(symbols[i].type) == "SCTN" || symbols[i].ndx == -1 || bindTypeToString(symbols[i].bind) == "LOC") continue;
			
			string sectionName = symbols[symbols[i].ndx].name;
			
			for(SectionHeaderElem& elem: finalSectionHeader){
				if(elem.name == sectionName){

					int currFileSectionOffset = 0;
					vector<pair<string, int>> fileNames = mpSectionFilesOffsets.find(sectionName)->second;

					for(auto& fileOffset: fileNames){
						if(fileOffset.first == currFileName){
							currFileSectionOffset = fileOffset.second;
							break;
						}
					}
					
					Symbol newSymbol(symbolTable.size(), elem.address + currFileSectionOffset + symbols[i].value, 0, 0, symbols[i].bind, elem.num, symbols[i].name);
					
					// check if there are multiple symbol definitions
					for(auto& entry: symbolTable){
						if(symbolTypeToString(entry.type) == "NOTYP" && bindTypeToString(entry.bind) == "GLOB" && entry.name == symbols[i].name){
							cout << "ERROR: " << "Multiple definitions of symbol: " << entry.name << "\n";
							exit(-1); 
						}
					}
					
					symbolTable.push_back(newSymbol);

					break;
				}
			}
		}
	}
}

void Linker::resolving(){
	cout << "RESOLVING STARTED\n";

	for(auto& it: mpRelocationTable){
    	int pos = it.first.find(":");
    	string fileName = it.first.substr(0, pos);
    	string relocTableName = it.first.substr(pos + 1);
    	string sectionName = relocTableName.substr(5);

    	
    	for(RelocationRecord& rr: it.second){
    		int p = rr.offset;
    		int s = 0;
    		int a = rr.addend;
    		string symName;
    		int currFileSectionOffset = 0;
			vector<pair<string, int>> fileNames = mpSectionFilesOffsets.find(sectionName)->second;

			for(auto& fileOffset: fileNames){ //IMPORTANT
				if(fileOffset.first == fileName){
					currFileSectionOffset = fileOffset.second;
					break;
				}
			}

    		for(SectionHeaderElem& elem: finalSectionHeader){
				if(elem.name == sectionName){
					p += elem.address + currFileSectionOffset; //IMPORTANT
					break;
				}
			}
				
			vector<Symbol>& currSymbolTable = mpSymbolTable.find(fileName)->second;

			for(Symbol& symbol: currSymbolTable){
				
				if(symbol.num == rr.symbol){
					
					symName = symbol.name;
					break;
				}
			}
			
			for(Symbol& symbol: symbolTable){
				if(symbol.name == symName){
					
					s = symbol.value;
				}
			}
			
    		if(relocationTypeToString(rr.type) == "R_X86_64_PC32"){
    			for(auto& it: outputData){
					
    				if(it.first == p) it.second = s + a;
				}
				continue;
			}
			
			if(relocationTypeToString(rr.type) == "R_X86_64_32" || relocationTypeToString(rr.type) == "R_X86_64_32S"){
				for(auto& it: outputData){
					
    				if(it.first == p) it.second = s + a;
				}
				continue;
			}
		}
	}
}

void Linker::place(){
	
	vector<pair<string, unsigned int>> vc1;
	for(auto& it: mpSectionAddress) vc1.push_back(it);

	sort(vc1.begin(), vc1.end(), 
		[](const pair<string, unsigned int>& a, const pair<string, unsigned int>& b){
			return a.second < b.second;
		}
	);
	
	vector<pair<string, int>> vc2;
	for(auto& it: sections) vc2.push_back(it);

	sort(vc2.begin(), vc2.end(), 
		[](const pair<string, unsigned int>& a, const pair<string, unsigned int>& b){
			return a.second < b.second;
		}
	);
	
	for(auto& it: vc1){
		
		location = it.second;
		
		mpSectionPos.insert(make_pair(it.first, location));
		
		vector<uint8_t>& currSectionData = mpSectionData.find(it.first)->second;
		
		int currData;
		
		
		for(int i = 0; i < currSectionData.size(); i += 4, location += 4){
			
			currData = 0;
			
			currData |= ((int)currSectionData[i]) << 24;
			
			if(i + 1 < currSectionData.size()){
				
				currData |= ((int)currSectionData[i + 1]) << 16;
			}
			if(i + 2 < currSectionData.size()){
				
				currData |= ((int)currSectionData[i + 2]) << 8;
			}
				
			if(i + 3 < currSectionData.size()){
				
				currData |= ((int)currSectionData[i + 3]);
			}
				
			
			outputData.push_back(make_pair(location, currData));
		}
		
    }
	
	for(auto& it: vc2){
		
		mpSectionPos.insert(make_pair(it.first, location));
		
		vector<uint8_t>& currSectionData = mpSectionData.find(it.first)->second;
		
		int currData;
		
		for(int i = 0; i < currSectionData.size(); i += 4, location += 4){
			
			currData = 0;
			
			currData |= ((int)currSectionData[i]) << 24;
			
			if(i + 1 < currSectionData.size()){
				
				currData |= ((int)currSectionData[i + 1]) << 16;
			}
			if(i + 2 < currSectionData.size()){
				
				currData |= ((int)currSectionData[i + 2]) << 8;
			}
				
			if(i + 3 < currSectionData.size()){
				
				currData |= ((int)currSectionData[i + 3]);
			}
			
			outputData.push_back(make_pair(location, currData));
		}
  }
	
}

void Linker::readSymbolTable(ifstream& file, string fileName){
	
	int symbolTableSize;
	file.read((char*)(&symbolTableSize), sizeof(symbolTableSize));
	
	int counter = 0;
	
	vector<Symbol> symbolTable;
	
	while(counter < symbolTableSize){
		int num;
		int value;
		int size;
		uint8_t type; 
		uint8_t bind; 
		int ndx; 
		int nameSize;
		string name;
		
		file.read((char*)(&num), sizeof(num));
		file.read((char*)(&value), sizeof(value));
		file.read((char*)(&size), sizeof(size));
		file.read((char*)(&type), sizeof(type));
		file.read((char*)(&bind), sizeof(bind));
		file.read((char*)(&ndx), sizeof(ndx));
			
			
		file.read((char*)(&nameSize), sizeof(nameSize));
		cout << fileName << "\n";
		cout << nameSize << "\n";
		name.resize(nameSize);
		file.read(&name[0], nameSize);
		
		counter += 4 * sizeof(int) + 2 * sizeof(uint8_t) + sizeof(int) + name.size();
		
		Symbol symbol(num, value, size, type, bind, ndx, name);
		symbolTable.push_back(symbol);
	}
	
	mpSymbolTable.insert(make_pair(fileName, symbolTable));
}


void Linker::readSectionHeader(ifstream& file, string fileName){
	
	int sectionHeaderSize;
	file.read((char*)(&sectionHeaderSize), sizeof(sectionHeaderSize));
	
	int counter = 0;
	
	vector<SectionHeaderElem> sectionHeader;
	
	while(counter < sectionHeaderSize){
		int num;
		int address;
		int size;
		int nameSize;
		string name;
		
		
		file.read((char*)(&num), sizeof(num));
		file.read((char*)(&address), sizeof(address));
		file.read((char*)(&size), sizeof(size));
		
		file.read((char*)(&nameSize), sizeof(nameSize));
		name.resize(nameSize);
		file.read(&name[0], nameSize);
		
		SectionHeaderElem elem;
		elem.num = num;
		elem.address = address;
		elem.size = size;
		elem.name = name;
		elem.currSize = 0;
		
		counter += 3 * sizeof(int) + sizeof(int) + name.size();
		
		sectionHeader.push_back(elem);
	}
	
	mpSectionHeader.insert(make_pair(fileName, sectionHeader));
}

void Linker::readSection(ifstream& file, string fileName, SectionHeaderElem& elem){
	
	vector<uint8_t> data;
	string sectionName = elem.name;
	
	
	for(int i = 0; i < elem.size; i++){
		uint8_t dataByte;
		
		file.read((char*)(&dataByte), sizeof(dataByte));
	
		data.push_back(dataByte);
	}
	
	if(mpSectionData.find(sectionName) != mpSectionData.end()){
		for(int i = 0; i < data.size(); i++){
			mpSectionData[sectionName].push_back(data[i]);
		}
	}
	else{
		
		mpSectionData.insert(make_pair(sectionName, data));
	}

	if(mpSectionFilesOffsets.find(sectionName) != mpSectionFilesOffsets.end()){ //remember the order of files
		vector<pair<string, int>>& vc = mpSectionFilesOffsets.find(sectionName)->second;
		vc.push_back(make_pair(fileName, mpSectionOffset[sectionName]));
		mpSectionOffset[sectionName] += elem.size;
	}
	else{
		vector<pair<string, int>> vc;
		vc.push_back(make_pair(fileName, 0));
		mpSectionOffset.insert(make_pair(sectionName, elem.size));
		mpSectionFilesOffsets.insert(make_pair(sectionName, vc));
	}
	
	if(mpSectionAddress.find(sectionName) == mpSectionAddress.end() && sections.find(sectionName) == sections.end()){
		sections.insert(make_pair(sectionName, sections.size()));
	}
	
}

int Linker::readNumberOfRelocationTables(ifstream& file, string fileName){
	int numberOfRelocationTables;
	file.read((char*)(&numberOfRelocationTables), sizeof(numberOfRelocationTables));
	return numberOfRelocationTables;
}

void Linker::readRelocationTable(ifstream& file, string fileName){
	
	int relocationTableSize;
	int relocationTableNameSize;
	string relocationTableName;
	
	file.read((char*)(&relocationTableSize), sizeof(relocationTableSize));
	
	file.read((char*)(&relocationTableNameSize), sizeof(relocationTableNameSize));
	relocationTableName.resize(relocationTableNameSize);
	file.read(&relocationTableName[0], relocationTableNameSize);

	int rows = relocationTableSize / (4 + 1 + 4 + 4);
	
	vector<RelocationRecord> vc;
	
	for(int i = 0; i < rows; i++){
		int offset;
		uint8_t type;
		int symbol;
		int addend;
		
		file.read((char*)(&offset), sizeof(offset));
		file.read((char*)(&type), sizeof(type));
		file.read((char*)(&symbol), sizeof(symbol));
		file.read((char*)(&addend), sizeof(addend));
		
		RelocationRecord rr;
		rr.offset = offset;
		rr.type = type;
		rr.symbol = symbol;
		rr.addend = addend;
		
		vc.push_back(rr);
	}
	
	mpRelocationTable.insert(make_pair(fileName + ":" + relocationTableName, vc));

}

void Linker::outputToFiles(){
	
	int pos = ofile.find(".");
	string textFileName = ofile.substr(0, pos) + ".txt";
	
	file.open(ofile, std::fstream::out);
	textFile.open(textFileName, std::fstream::out);
	
	int cnt = 0;
	for(auto& it: outputData){
		
		//output address
		file << mpHex[((it.first & 0xf0000000) >> 28) & 0x0000000f];
		file << mpHex[((it.first & 0x0f000000) >> 24) & 0x0000000f];
		
		file << mpHex[((it.first & 0x00f00000) >> 20) & 0x0000000f];
		file << mpHex[((it.first & 0x000f0000) >> 16) & 0x0000000f];
		
		file << mpHex[((it.first & 0x0000f000) >> 12) & 0x0000000f];
		file << mpHex[((it.first & 0x00000f00) >> 8) & 0x0000000f];
		
		file << mpHex[((it.first & 0x000000f0) >> 4) & 0x0000000f];
		file << mpHex[((it.first & 0x0000000f) >> 0) & 0x0000000f];
		
		//output value
		file << mpHex[((it.second & 0xf0000000) >> 28) & 0x0000000f];
		file << mpHex[((it.second & 0x0f000000) >> 24) & 0x0000000f];
		
		file << mpHex[((it.second & 0x00f00000) >> 20) & 0x0000000f];
		file << mpHex[((it.second & 0x000f0000) >> 16) & 0x0000000f];
		
		file << mpHex[((it.second & 0x0000f000) >> 12) & 0x0000000f];
		file << mpHex[((it.second & 0x00000f00) >> 8) & 0x0000000f];
		
		file << mpHex[((it.second & 0x000000f0) >> 4) & 0x0000000f];
		file << mpHex[((it.second & 0x0000000f) >> 0) & 0x0000000f];
		
		file << "\n";
		
		//output to textFile
		
		textFile << mpHex[((it.first & 0xf0000000) >> 28) & 0x0000000f];
		textFile << mpHex[((it.first & 0x0f000000) >> 24) & 0x0000000f];
		
		textFile << mpHex[((it.first & 0x00f00000) >> 20) & 0x0000000f];
		textFile << mpHex[((it.first & 0x000f0000) >> 16) & 0x0000000f];
		
		textFile << mpHex[((it.first & 0x0000f000) >> 12) & 0x0000000f];
		textFile << mpHex[((it.first & 0x00000f00) >> 8) & 0x0000000f];
		
		textFile << mpHex[((it.first & 0x000000f0) >> 4) & 0x0000000f];
		textFile << mpHex[((it.first & 0x0000000f) >> 0) & 0x0000000f];
		
		textFile << ":\t";
		
		textFile << mpHex[((it.second & 0x000000f0) >> 4) & 0x0000000f];
		textFile << mpHex[((it.second & 0x0000000f) >> 0) & 0x0000000f];
		
		textFile << " ";
		
		textFile << mpHex[((it.second & 0x0000f000) >> 12) & 0x0000000f];
		textFile << mpHex[((it.second & 0x00000f00) >> 8) & 0x0000000f];
		
		textFile << " ";
		
		textFile << mpHex[((it.second & 0x00f00000) >> 20) & 0x0000000f];
		textFile << mpHex[((it.second & 0x000f0000) >> 16) & 0x0000000f];
		
		textFile << " ";
		
		textFile << mpHex[((it.second & 0xf0000000) >> 28) & 0x0000000f];
		textFile << mpHex[((it.second & 0x0f000000) >> 24) & 0x0000000f];
		
		textFile << "\n";
		
	}
}
