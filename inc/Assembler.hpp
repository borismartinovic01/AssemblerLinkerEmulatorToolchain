#ifndef _ASSEMBLER_HPP
#define _ASSEMBLER_HPP

#include "structsDefs.hpp"
#include "instructionsCode.hpp"
#include "utils.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <queue>
#include <fstream>

using namespace std;

class Assembler{
private:
	string ofile;
	string ofileTxt;
	int currentPass = 1; //1 - first pass; 2 - second pass; 3 - assembling end
	vector<Line> lines;
	int currSectionIdx = -1;
	ofstream file;
	fstream textFile;
	string assemblingMessage;
	int symbolTableSize = 0;
	
	vector<Section> sectionTable;
	vector<Symbol> symbolTable;
	vector<pair<string, vector<RelocationRecord> > > relocationTables;
	
	unordered_map<int, string> mpHex;
	
	void initSymbolTable();
	void globalDirective(Line line);
	void externDirective(Line line);
	void sectionDirective(Line line);
	void wordDirective(Line line);
	void skipDirective(Line line);
	void endDirective();
	void assemblyLine(Line line);
	void getSymbols(SymbolsList& symbolsList, vector<string>& vc);
	void getSymbolsAndLiterals(SymbolsList& symbolsList, vector<string>& vc, vector<bool>& isSymbol);
	void outputSectionHeader();
	void outputSymbolTable();
	void outputSectionData();
	void outputRelocationTables();
	void outputInstructionCode(int code);
	void outputIntValue(int val);
	void jmpInstruction(Line line, int code);
	void addLabel(string label);
	void ldInstruction(Line line);
	void stInstruction(Line line);
	void ldImmed(Line line);
	void ldMem(Line line);
	void ldRegdir(Line line);
	void ldRegind(Line line);
	void ldRegindpom(Line line);
	void stMem(Line line);
	void stRegind(Line line);
	void stRegindpom(Line line);
	
public:
	Assembler(vector<Line> lines, string ofile)
		:lines(lines), ofile(ofile){}
	
	void assembly();
	void getAssemblyMessage();
};

#endif
