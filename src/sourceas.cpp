#include <iostream>
#include <string>
#include <cstring>

#include "../inc/Assembler.hpp"
#include "../machine.tab.hpp"

extern int yylex();
extern void yylex_destroy();
extern FILE *yyin;
extern int counter;

using namespace std;


int checkArguments(int argc, char* argv[], string& ifile, string& ofile){
	if(argc == 2){
		ifile = argv[1];
		ofile = "asemblerResult.o";
		return 0;
	}
	if(argc == 4){
		if(strcmp(argv[1], "-o") != 0) return -1;
		ifile = argv[3];
		ofile = argv[2];
		return 0;
	}
	return -1;
}

void yyerror(vector<Line>& lines, const char *s) {
    string error = "ERROR LINE: " + to_string(counter) + "; INSTRUCTION: " + to_string(lines.size()) + ": " + s;
    throw std::runtime_error(error);
}

int getLines(string ifile, vector<Line>& lines){
	
	FILE* inputFile = fopen(ifile.c_str(), "r");
	yyin = inputFile;
	if(yyin == NULL){
		return -1;
	}
	
	int succ = yyparse(lines);
	
	yylex_destroy();
	fclose(inputFile);
	yyin = NULL;
	return succ;
}

int main(int argc, char* argv[]) {
	
	string ifile;
	string ofile;
	
	cout << "Assembler started" << "\n";

	if(checkArguments(argc, argv, ifile, ofile) == -1){
		cout << "Bad arguments" << "\n";
		return -1;
	}
	
	vector<Line> lines;
	int succ = getLines(ifile, lines);
	if(succ == 2){
		cout << "Out of memory";
		return -1;
	}
	if(succ != 0){
		cout << "Unknown error";
		return -1;
	}

	cout << "Starting assembling" << "\n";
	Assembler assembler(lines, ofile);
	assembler.assembly();
	cout << "FINISHED ASSEMBLING\n";
	return 0;
}
