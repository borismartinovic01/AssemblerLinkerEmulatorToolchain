#ifndef _EMULATOR_HPP
#define _EMULATOR_HPP

#include "instructionsCode.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cstdio>

using namespace std;


class Emulator{
private:
	
	unordered_map<unsigned int, int> memory;
	
	struct ProgramState{
    	vector<int> registers = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x40000000};
    	vector<int> csr = {0, 0, 0};
	};
	
	ProgramState programState;
	
	void executeInstruction(int dcInstruction);
	int getOc(int instruction);
	int getMod(int instruction);
	unsigned int getRega(int instruction);
	unsigned int getRegb(int instruction);
	unsigned int getRegc(int instruction);
	unsigned getDispl(int instruction);
	
	void haltInstruction(int currInstruction);
	void intInstruction(int currInstruction);
	void callImmedInstruction(int currInstruction);
	void callMemInstruction(int currInstruction);
	void jmpImmedInstruction(int currInstruction);
	void beqImmedInstruction(int currInstruction);
	void bneImmedInstruction(int currInstruction);
	void bgtImmedInstruction(int currInstruction);
	void jmpMemInstruction(int currInstruction);
	void beqMemInstruction(int currInstruction);
	void bneMemInstruction(int currInstruction);
	void bgtMemInstruction(int currInstruction);
	void xchgInstruction(int currInstruction);
	void addInstruction(int currInstruction);
	void subInstruction(int currInstruction);
	void mulInstruction(int currInstruction);
	void divInstruction(int currInstruction);
	void notInstruction(int currInstruction);
	void andInstruction(int currInstruction);
	void orInstruction(int currInstruction);
	void xorInstruction(int currInstruction);
	void shlInstruction(int currInstruction);
	void shrInstruction(int currInstruction);
	void pushInstruction(int currInstruction);
	void csrrdInstruction(int currInstruction);
	void popInstruction(int currInstruction);
	void csrwrInstruction(int currInstruction);
	void popStatusInstruction(int currInstruction);
	void ldMemInstruction(int currInstruction);
	void ldRegdirInstruction(int currInstruction);
	void stRegindInstruction(int currInstruction);
	void stMemInstruction(int currInstruction);
	
	int read4Bytes(unsigned int address);
	void write4Bytes(unsigned int address, int value);
	
public:
	Emulator(unordered_map<unsigned int, int>& memory)
		:memory(memory){}
	
	void emulate();
};

#endif
